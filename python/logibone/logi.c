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
	unsigned char type_size = 1 ;
	uint8_t * rx ;
	if(!PyArg_ParseTuple(arg, "ll|b", &offset, &size, &type_size))
		return NULL;				
	switch(type_size){
		case 1:
			rx = (uint8_t *) malloc(size*type_size);
        		logi_read(rx, size, offset);
        		transferTuple = PyTuple_New(size);
        		for(i=0;i<size;i++)
                		PyTuple_SetItem(transferTuple, i, Py_BuildValue("i",((uint8_t *) rx)[i]));
			free(rx);
			break ;
		case 2:
			rx = (uint8_t *) malloc(size*type_size);
                        logi_read(rx, size*2, offset);
                        transferTuple = PyTuple_New(size);
                        for(i=0;i<size;i++)
                                PyTuple_SetItem(transferTuple, i, Py_BuildValue("i",((uint16_t *) rx)[i]));
                        free(rx);
			break ;
		case 4:
			rx = (uint8_t *) malloc(size*type_size);
                        logi_read(rx, size*4, offset);
                        transferTuple = PyTuple_New(size);
                        for(i=0;i<size;i++)
                                PyTuple_SetItem(transferTuple, i, Py_BuildValue("i",((uint32_t *) rx)[i]));
                        free(rx);
			break ;
		default :
			pabort("type_size argument can only be 1, 2, 4\n");
			break;
		
		
	}
	return transferTuple ;
}

static PyObject* logiWrite(PyObject* self, PyObject* arg)
{
	PyObject* transferTuple;
	unsigned int offset, returnVal ;
	unsigned char type_size = 1 ;

	if(!PyArg_ParseTuple(arg, "lO|b", &offset, &transferTuple, &type_size))
		return NULL;

	if(!PyTuple_Check(transferTuple))
		pabort("Only accepts a single tuple as an argument\n");

	if(type_size != 1 && type_size != 2 && type_size != 4)
		pabort("type_size argument can only be 1, 2, 4\n");
	uint32_t tupleSize = PyTuple_Size(transferTuple);
	uint8_t tx[tupleSize*type_size];
	PyObject* tempItem;
	uint32_t i=0;
	while(i < tupleSize)
	{
		tempItem = PyTuple_GetItem(transferTuple, i);		//
		if(!PyInt_Check(tempItem))
		{
			pabort("non-integer contained in tuple\n");
		}
		switch(type_size){
			case 1:
				tx[i] = (uint8_t)PyInt_AsSsize_t(tempItem);
				break ;
			case 2:
				((uint16_t*)tx)[i] = (uint16_t)PyInt_AsSsize_t(tempItem);
				break ;
			case 4:
				((uint32_t*)tx)[i] = (uint32_t)PyInt_AsSsize_t(tempItem);
				break ;
			default:
				break ;

			
		}
		i++;

	}
	returnVal = logi_write(tx, tupleSize*type_size, offset);
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
