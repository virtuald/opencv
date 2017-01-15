/**
    Utility API for converting Python types to/from OpenCV types
    
    This is useful when you have a python module that links to a C++ library that
    uses OpenCV, and you need to convert things to/from python.
    
    For now, this only supports converting Mat objects, but in the future
    other types of conversions may be supported. 
    
    This code is copied wholesafe from numpy's generate_numpy_api.py
    
Copyright (c) 2005-2016, NumPy Developers.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials provided
       with the distribution.

    * Neither the name of the NumPy Developers nor the names of any
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __PYCV2_API
#define __PYCV2_API

#include <Python.h>
#include <opencv2/core/core.hpp>

#if defined(PY_OPENCV_UNIQUE_SYMBOL)
#define PyOpenCV_API PY_OPENCV_UNIQUE_SYMBOL
#endif

#if defined(NO_IMPORT) || defined(NO_IMPORT_OPENCV)
extern void **PyOpenCV_API;
#else
#if defined(PY_OPENCV_UNIQUE_SYMBOL)
void **PyOpenCV_API;
#else
static void **PyOpenCV_API=NULL;
#endif
#endif

#if !defined(NO_IMPORT_OPENCV) && !defined(NO_IMPORT)
static int
_import_opencv(void)
{
  //int st;
  PyObject *cv2 = PyImport_ImportModule("cv2");
  PyObject *c_api = NULL;

  if (cv2 == NULL) {
      PyErr_SetString(PyExc_ImportError, "cv2 failed to import");
      return -1;
  }
  c_api = PyObject_GetAttrString(cv2, "_OPENCV_API");
  Py_DECREF(cv2);
  if (c_api == NULL) {
      PyErr_SetString(PyExc_AttributeError, "_OPENCV_API not found");
      return -1;
  }

#if PY_VERSION_HEX >= 0x03000000
  if (!PyCapsule_CheckExact(c_api)) {
      PyErr_SetString(PyExc_RuntimeError, "_OPENCV_API is not PyCapsule object");
      Py_DECREF(c_api);
      return -1;
  }
  PyOpenCV_API = (void **)PyCapsule_GetPointer(c_api, NULL);
#else
  if (!PyCObject_Check(c_api)) {
      PyErr_SetString(PyExc_RuntimeError, "_OPENCV_API is not PyCObject object");
      Py_DECREF(c_api);
      return -1;
  }
  PyOpenCV_API = (void **)PyCObject_AsVoidPtr(c_api);
#endif
  Py_DECREF(c_api);
  if (PyOpenCV_API == NULL) {
      PyErr_SetString(PyExc_RuntimeError, "_OPENCV_API is NULL pointer");
      return -1;
  }

  /* TODO? Perform runtime check of C API version */
  /*
  if (NPY_VERSION != PyArray_GetNDArrayCVersion()) {
      PyErr_Format(PyExc_RuntimeError, "module compiled against "\
             "ABI version 0x%%x but this version of numpy is 0x%%x", \
             (int) NPY_VERSION, (int) PyArray_GetNDArrayCVersion());
      return -1;
  }
  if (NPY_FEATURE_VERSION > PyArray_GetNDArrayCFeatureVersion()) {
      PyErr_Format(PyExc_RuntimeError, "module compiled against "\
             "API version 0x%%x but this version of numpy is 0x%%x", \
             (int) NPY_FEATURE_VERSION, (int) PyArray_GetNDArrayCFeatureVersion());
      return -1;
  }
  */

  /*
   * TODO? Perform runtime check of endianness and check it matches the one set by
   * the headers (npy_endian.h) as a safeguard
   */
  /*
  st = PyArray_GetEndianness();
  if (st == NPY_CPU_UNKNOWN_ENDIAN) {
      PyErr_Format(PyExc_RuntimeError, "FATAL: module compiled as unknown endian");
      return -1;
  }
#if NPY_BYTE_ORDER == NPY_BIG_ENDIAN
  if (st != NPY_CPU_BIG) {
      PyErr_Format(PyExc_RuntimeError, "FATAL: module compiled as "\
             "big endian, but detected different endianness at runtime");
      return -1;
  }
#elif NPY_BYTE_ORDER == NPY_LITTLE_ENDIAN
  if (st != NPY_CPU_LITTLE) {
      PyErr_Format(PyExc_RuntimeError, "FATAL: module compiled as "\
             "little endian, but detected different endianness at runtime");
      return -1;
  }
#endif
  */
  
  return 0;
}

#if PY_VERSION_HEX >= 0x03000000
#define OPENCV_IMPORT_RETVAL NULL
#else
#define OPENCV_IMPORT_RETVAL
#endif

#define import_opencv() {if (_import_opencv() < 0) {PyErr_Print(); PyErr_SetString(PyExc_ImportError, "cv2 failed to import"); return OPENCV_IMPORT_RETVAL; } }

#define import_opencv1(ret) {if (_import_opencv() < 0) {PyErr_Print(); PyErr_SetString(PyExc_ImportError, "cv2 failed to import"); return ret; } }

#define import_opencv2(msg, ret) {if (_import_opencv() < 0) {PyErr_Print(); PyErr_SetString(PyExc_ImportError, msg); return ret; } }

/**
  Exported API, must call import_opencv first!
*/

#define PyOpenCV_MatToNdarray \
        (*(PyObject * (*)(const cv::Mat&)) \
         PyOpenCV_API[0])
#define PyOpenCV_NdarrayToMat \
        (*(bool (*)(PyObject *, cv::Mat&, const char *)) \
         PyOpenCV_API[1])
         
#define PyOpenCV_UMatToNdarray \
       (*(PyObject * (*)(const cv::UMat&)) \
        PyOpenCV_API[2])
#define PyOpenCV_NdarrayToUMat \
       (*(bool (*)(PyObject *, cv::UMat&, const char *)) \
        PyOpenCV_API[3])

#endif
#endif
