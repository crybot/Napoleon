{ pkgs ? import <nixpkgs> {} }:

let
  # 1) Override onnxruntime to enable CUDA Execution Provider
  onnxruntimeCUDAPkg = pkgs.onnxruntime.override {
    cudaSupport = true;                   # ← turn on onnxruntime_USE_CUDA :contentReference[oaicite:7]{index=7}
  };
in

pkgs.mkShell {
  buildInputs = [
    onnxruntimeCUDAPkg                   # Provides libonnxruntime & libonnxruntime_providers_cuda
    pkgs.cudaPackages.cudatoolkit        # CUDA Toolkit (libcudart, nvcc headers, etc.) :contentReference[oaicite:8]{index=8}
    pkgs.cudaPackages.cudnn              # cuDNN libraries                                      :contentReference[oaicite:9]{index=9}
    # pkgs.cudaPackages.tensorrt
  ];

  # 2) Ensure dynamic loader sees the CUDA provider library
  shellHook = ''
    export LD_LIBRARY_PATH=${onnxruntimeCUDAPkg}/lib:$LD_LIBRARY_PATH
  '';
}
