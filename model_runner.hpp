#pragma once
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>

class ModelRunner {
public:
    /// model_path: path to .onnx file
    /// gpu_device: set >=0 to enable CUDA on that device, else CPU only
    ModelRunner(const std::string& model_path, int gpu_device = -1);

    /// Runs inference and returns the first scalar output.
    /// data1, shape1: first tensor (e.g. 1×12×8×8)
    /// data2, shape2: second tensor (e.g. 1×3)
    float run(const std::vector<float>& data1,
              const std::vector<int64_t>& shape1,
              const std::vector<float>& data2,
              const std::vector<int64_t>& shape2);

private:
    Ort::Env env_;                                    // Runtime environment
    Ort::SessionOptions opts_;                        // Session options
    Ort::Session session_;                            // Inference session
    Ort::AllocatorWithDefaultOptions allocator_;      // For name allocation

    // Hold AllocatedStringPtr to keep names alive until after Run()
    std::vector<Ort::AllocatedStringPtr> in_name_ptrs_;
    std::vector<Ort::AllocatedStringPtr> out_name_ptrs_;

    // Raw C-string arrays for Run()
    std::vector<const char*> in_names_;
    std::vector<const char*> out_names_;
};
