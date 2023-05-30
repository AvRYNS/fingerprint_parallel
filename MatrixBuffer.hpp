#pragma once

#include "CL/opencl.hpp"
#include "OclException.hpp"
#include "OclInfo.hpp"
#include <memory>

/**
 * @brief Class represents Matrix. May contains related Opencl Buffer object.
 *
 */
template <typename T>
class MatrixBuffer {
  private:
    T *_data = nullptr;
    unsigned int _width;
    unsigned int _height;
    unsigned int _len;
    cl::Buffer *_buffer = nullptr;

  public:
    /**
     * @brief Create MatruxBuffer with width, and height.
     * @param width width of image. Number of pixels in row.
     * @param height height of image. Number of pixels in column.
     */
    MatrixBuffer(int width, int height)
        : _width(width), _height(height), _len(width * height) {

        _data = new T[_len];
    };

    ~MatrixBuffer() {
        delete[] _data;
        _data = nullptr;
        if (_buffer != nullptr)
            delete _buffer;
    }

    /**
     * @brief Initialize OpenCL Buffer for matrix.
     * @param ctx cl::Context object
     * @param memFlag flag used for OpenCL memory access policy. Default = CL_MEM_READ_WRITE
     */
    void createBuffer(cl::Context ctx, cl_mem_flags memFlag = CL_MEM_READ_WRITE) {
        cl_int err = CL_SUCCESS;
        _buffer = new cl::Buffer(ctx, memFlag, _len * sizeof(T), nullptr, &err);

        if (err != CL_SUCCESS) {
            throw OclException("Error while creating OCL buffer in MatrixBuffer.", err);
        }
    }

    /**
     * @brief Get width of matrix.
     * @return Width of matrix
     */
    unsigned int getWidth() { return _width; }

    /**
     * @brief Get height of matrix.
     * @return Height of matrix
     */
    unsigned int getHeight() { return _height; }

    /**
     * @brief Get numbers of elements in matrix. (width*height)
     * @return width*height
     */
    unsigned int getLen() { return _len; }

    /**
     * @brief  Get pointer that points first element of matrix.
     * @return Pointer to first element.
     */
    T *getData() {
        return _data;
    }

    /**
     * @brief Get Related OpenCL Buffer.
     * @return Related OpenCl Buffer
     */
    cl::Buffer *getClBuffer() { return _buffer; }

    /**
     * @brief Copy Host memory to Gpu.
     * @param oclInfo OclInfo contains valid queue.
     * @param blocking if false, only enqueue job and continue. Default=true.
     */
    void toGpu(OclInfo &oclInfo, bool blocking = true) {
        cl_int err = oclInfo.queue.enqueueWriteBuffer(*getClBuffer(), blocking, 0, getLen() * sizeof(T), (void*) getData(), nullptr, nullptr);
        if (err)
            throw OclException("Error enqueueWriteBuffer", err);
    }

    /**
     * @brief Copy Gpu memory to host.
     * @param oclInfo OclInfo contains valid queue.
     * @param blocking if false, only enqueue job and continue. Default=true.
     */
    void toHost(OclInfo &oclInfo, bool blocking = true) {
        cl_int err = oclInfo.queue.enqueueReadBuffer(*getClBuffer(), blocking, 0, getLen() * sizeof(T), (void*) getData(), nullptr, nullptr);
        if (err)
            throw OclException("Error enqueueReadBuffer", err);
    }
};
