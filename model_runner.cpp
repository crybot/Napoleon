#include "model_runner.hpp"
#include <cuda_runtime.h>
// #ifdef USE_CUDA
// #include <cuda_provider_factory.h>   // for AppendExecutionProvider_CUDA
// #endif

ModelRunner::ModelRunner(const std::string& model_path, int gpu_device)
  : env_(ORT_LOGGING_LEVEL_WARNING, "ModelRunner"),
  opts_() {
  // Optional: enable CUDA if requested

  if (gpu_device >= 0) {
    OrtCUDAProviderOptions cuda_opts;
    cuda_opts.device_id = gpu_device;
    opts_.AppendExecutionProvider_CUDA(cuda_opts);  // GPU EP registration :contentReference[oaicite:2]{index=2}
  }

  opts_.SetIntraOpNumThreads(1);
  opts_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
  opts_.DisableProfiling();

  // Create the session
  session_.emplace(env_, model_path.c_str(), opts_);
  // session_ = Ort::Session(env_, model_path.c_str(), opts_);

  Ort::Session& session = session_.value(); // Get a reference to the session
  
  // Fetch input names
  size_t in_count = session.GetInputCount();
  in_name_ptrs_.reserve(in_count);
  in_names_.reserve(in_count);
  for (size_t i = 0; i < in_count; ++i) {
    auto name_ptr = session.GetInputNameAllocated(i, allocator_);
    in_names_.push_back(name_ptr.get());
    in_name_ptrs_.push_back(std::move(name_ptr));  // keep buffer alive
  }

  // Fetch output names
  size_t out_count = session.GetOutputCount();
  out_name_ptrs_.reserve(out_count);
  out_names_.reserve(out_count);
  for (size_t i = 0; i < out_count; ++i) {
    auto name_ptr = session.GetOutputNameAllocated(i, allocator_);
    out_names_.push_back(name_ptr.get());
    out_name_ptrs_.push_back(std::move(name_ptr));
  }

  // Create CPU memory info (tensors will be copied to GPU internally if CUDA EP is active)
  auto mem_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

  std::vector<float> mock_bitboards(1 * 12 * 8 * 8, 0.0f);
  std::vector<float> mock_aux(1 * 3, 0.0f);
  std::vector<int64_t> shape1{1, 12, 8, 8};
  std::vector<int64_t> shape2{1, 3};

  // Build mock input Ort::Value objects
  // TODO: make the tensors backed by memory on the CUDA device so that when calling `run` we can simply update the
  // memory on the cuda device without reallocating tensors or rebinding inputs
  Ort::Value bitboards_tensor = Ort::Value::CreateTensor<float>(
    mem_info,
    const_cast<float*>(mock_bitboards.data()), mock_bitboards.size(),
    shape1.data(), shape1.size()
  );
  Ort::Value aux_tensor = Ort::Value::CreateTensor<float>(
    mem_info,
    const_cast<float*>(mock_aux.data()), mock_aux.size(),
    shape2.data(), shape2.size()
  );
  
  // IO Binding
  io_binding_.emplace(session);
  auto& io_binding = io_binding_.value();
  
  // Bind input (mock values will be replaced by the first call to run)
  io_binding.BindInput(in_names_[0], bitboards_tensor);
  io_binding.BindInput(in_names_[1], aux_tensor);

  // Bind output (let ORT allocate memory on device)
  Ort::MemoryInfo output_mem_info("Cuda", OrtDeviceAllocator, gpu_device, OrtMemTypeDefault);
  io_binding.BindOutput(out_names_[0], output_mem_info);

}

float ModelRunner::run(const std::vector<float>& data1,
                       const std::vector<int64_t>& shape1,
                       const std::vector<float>& data2,
                       const std::vector<int64_t>& shape2) {
  // Create CPU memory info (tensors will be copied to GPU internally if CUDA EP is active)
  auto mem_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

  // Build input Ort::Value objects
  Ort::Value t1 = Ort::Value::CreateTensor<float>(
    mem_info,
    const_cast<float*>(data1.data()), data1.size(),
    shape1.data(), shape1.size()
  );
  Ort::Value t2 = Ort::Value::CreateTensor<float>(
    mem_info,
    const_cast<float*>(data2.data()), data2.size(),
    shape2.data(), shape2.size()
  );

  auto& io_binding = io_binding_.value();
  io_binding.BindInput(in_names_[0], t1);
  io_binding.BindInput(in_names_[1], t2);
  
  Ort::RunOptions run_opts;
  session_.value().Run(run_opts, io_binding);

  auto host_output = std::array<float, 1>{0.0f};
  auto outputs = io_binding.GetOutputValues();
  auto out_ptr = outputs[0].GetTensorMutableData<float>();
  cudaMemcpy(host_output.data(), out_ptr, sizeof(float) * host_output.size(), cudaMemcpyDeviceToHost);

  return host_output[0];
}
