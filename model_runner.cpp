#include "model_runner.hpp"
// #ifdef USE_CUDA
// #include <cuda_provider_factory.h>   // for AppendExecutionProvider_CUDA
// #endif

ModelRunner::ModelRunner(const std::string& model_path, int gpu_device)
  : env_(ORT_LOGGING_LEVEL_WARNING, "ModelRunner"),
  opts_(),
  session_(env_, model_path.c_str(), opts_) {
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
  session_ = Ort::Session(env_, model_path.c_str(), opts_);

  // Fetch input names
  size_t in_count = session_.GetInputCount();
  in_name_ptrs_.reserve(in_count);
  in_names_.reserve(in_count);
  for (size_t i = 0; i < in_count; ++i) {
    auto name_ptr = session_.GetInputNameAllocated(i, allocator_);
    in_names_.push_back(name_ptr.get());
    in_name_ptrs_.push_back(std::move(name_ptr));  // keep buffer alive
  }

  // Fetch output names
  size_t out_count = session_.GetOutputCount();
  out_name_ptrs_.reserve(out_count);
  out_names_.reserve(out_count);
  for (size_t i = 0; i < out_count; ++i) {
    auto name_ptr = session_.GetOutputNameAllocated(i, allocator_);
    out_names_.push_back(name_ptr.get());
    out_name_ptrs_.push_back(std::move(name_ptr));
  }
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
  std::vector<Ort::Value> inputs;
  inputs.push_back(std::move(t1));
  inputs.push_back(std::move(t2));

  // Run inference
  auto outputs = session_.Run(
    Ort::RunOptions{nullptr},
    in_names_.data(),  inputs.data(),  inputs.size(),
    out_names_.data(), out_names_.size()
  );  // identical API to CPU usage :contentReference[oaicite:3]{index=3}

  // Extract and return the first scalar output
  float* out_ptr = outputs[0].GetTensorMutableData<float>();
  return out_ptr[0];
}
