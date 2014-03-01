#include <Python.h>
#include "logilib.h"

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static PyObject* logiRead(PyObject* self, PyObject* arg)
{
	PyObject* transferTuple;
	unsigned int offset, size, i ;
	if(!PyArg_ParseTuple(arg, "ll", &offset, &size))
		return NULL;					
	uint8_t rx[size];
	logi_read(rx, size, offset);
	transferTuple = PyTuple_New(size);
	for(i=0;i<size;i++)
		PyTuple_SetItem(transferTuple, i, Py_BuildValue("i",rx[i]));
	return transferTuple;
}

static PyObject* logiWrite(PyObject* self, PyObject* arg)
{
	PyObject* transferTuple;
	unsigned int offset, returnVal ;

	if(!PyArg_ParseTuple(arg, "lO", &offset, &transferTuple))		
		return NULL;					

	if(!PyTuple_Check(transferTuple))			
		pabort("Only accepts a single tuple as an argument\n");


	uint32_t tupleSize = PyTuple_Size(transferTuple);
	uint8_t tx[tupleSize];
	PyObject* tempItem;
	uint32_t i=0;
	while(i < tupleSize)
	{
		tempItem = PyTuple_GetItem(transferTuple, i);		//
		if(!PyInt_Check(tempItem))
		{
			pabort("non-integer contained in tuple\n");
		}
		tx[i] = (uint8_t)PyInt_AsSsize_t(tempItem);
		i++;

	}
	returnVal = logi_write(tx, tupleSize, offset);
	return Py_BuildValue("l", returnVal) ;
}


static PyMethodDef logiMethods[] =
{
	{"logiRead", logiRead, METH_VARARGS, "Read from with given offset"},
	{"logiWrite", logiWrite, METH_VARARGS, "Write to given offset"},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC

initlogi(void)
{
	(void) Py_InitModule("logi", logiMethods);
	logi_open();
}
