// CallPython.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CallPython.h"

#ifndef BOOST_PYTHON_STATIC_LIB
#define BOOST_PYTHON_STATIC_LIB
#endif

#include <boost/python.hpp>

CALLPYTHON_API void Call_Pay()
{
	Py_Initialize();

	PyObject *pModule = PyImport_ImportModule("message");
	boost::python::handle<>_module(pModule);
	boost::python::call_method<void>(pModule, "paySucceed");

	Py_Finalize();
}

CALLPYTHON_API void Call_Shipment(BOOL succeed)
{
	Py_Initialize();

	PyObject *pModule = PyImport_ImportModule("message");
	boost::python::handle<>_module(pModule);

	if (succeed)
		boost::python::call_method<void>(pModule, "shipmentSucceed");
	else
		boost::python::call_method<void>(pModule, "shipmentFailed");

	Py_Finalize();
}