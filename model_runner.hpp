#pragma once
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>
#include <optional>
#include <cuda_runtime.h>
#include <ranges>

class ModelRunner {
public:
  /// model_path: path to .onnx file
  /// gpu_device: set >=0 to enable CUDA on that device, else CPU only
  ModelRunner(const std::string& model_path, int gpu_device = -1);

  /// Runs inference and returns the first scalar output.
  /// data1, shape1: first tensor (e.g. 1×12×8×8)
  /// data2, shape2: second tensor (e.g. 1×3)
  void run(const std::vector<float>&,
           const std::vector<int64_t>&,
           const std::vector<float>&,
           const std::vector<int64_t>&);

  template<size_t Index, size_t N>
  auto fetchOutput() -> std::array<float, N>;

  template<size_t B, size_t N>
  auto batchCategorical(std::span<float, B * N>) -> std::array<int, B>;

  template<size_t Index, size_t B, size_t Classes>
  auto fetchCategorical() -> std::array<int, B>;

private:
  Ort::Env env_;                                    // Runtime environment
  Ort::SessionOptions opts_;                        // Session options
  std::optional<Ort::Session> session_;                            // Inference session
  Ort::AllocatorWithDefaultOptions allocator_;      // For name allocation
  std::optional<Ort::IoBinding> io_binding_;

  // Hold AllocatedStringPtr to keep names alive until after Run()
  std::vector<Ort::AllocatedStringPtr> in_name_ptrs_;
  std::vector<Ort::AllocatedStringPtr> out_name_ptrs_;

  // Raw C-string arrays for Run()
  std::vector<const char*> in_names_;
  std::vector<const char*> out_names_;
  std::vector<const float*> outputs_;

};

// TODO: move to appropriate file
template<typename T>
auto argmax(std::span<T> values) -> int {
  auto max_it = std::max_element(values.begin(), values.end());
  return std::distance(max_it, values.end());
}

template<size_t Index, size_t N> auto ModelRunner::fetchOutput() -> std::array<float, N> {
  auto output = outputs_.at(Index);
  auto host_output = std::array<float, N>{};
  cudaMemcpy(host_output.data(), output, sizeof(float) * host_output.size(), cudaMemcpyDeviceToHost);
  return host_output;
}

template<size_t B, size_t N>
auto ModelRunner::batchCategorical(std::span<float, B * N> batch) -> std::array<int, B> {
  std::array<int, B> indices{};
  // Chunk the flat span into B sub-ranges of length N
  auto rows = batch | std::views::chunk(N);

  // Transform each sub-range into its argmax index
  std::ranges::transform(
    rows,
    indices.begin(),
    [](auto row) {
      return argmax<float>(row);
    });

  return indices;
}

template<size_t Index, size_t B, size_t Classes>
auto ModelRunner::fetchCategorical() -> std::array<int, B> {
  auto output = fetchOutput<Index, B * Classes>();
  return batchCategorical<B, Classes>(output);
}
