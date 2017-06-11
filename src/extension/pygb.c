#include <stdio.h>
#include <Python.h>
#include "structmember.h"
#include "gameboy.h"

typedef struct {
    PyObject_HEAD
    struct gameboy gb;
} PyGameBoy;

static void PyGameBoy_dealloc(PyGameBoy *self) {
    Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject *PyGameBoy_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyGameBoy *self = (PyGameBoy *) type->tp_alloc(type, 0);
    
    gb_alloc_mem(&self->gb);
    gb_init(&self->gb, 0, 1);
    gb_reset(&self->gb);

    return (PyObject *) self;
}     

/* Load a ROM file.  Raises IOError exception if the file cannot be read. */
static PyObject *PyGameBoy_load_rom(PyGameBoy *self, PyObject *args) {
    char *rom_file;
    if (!PyArg_ParseTuple(args, "s", &rom_file))
        return NULL;

    struct gameboy *gb = &(self->gb);
    if (!load_rom(gb, rom_file))
        return PyErr_SetFromErrno(PyExc_IOError);
    gb_reset(gb);

    Py_INCREF(Py_None);
    return Py_None;
}

/* Run a certain number of cpu operations. */
static PyObject *PyGameBoy_run_ops(PyGameBoy *self, PyObject *args) {
    unsigned int ops;
    if (!PyArg_ParseTuple(args, "i", &ops))
        return NULL;

    struct gameboy *gb = &(self->gb);
    gb_run_ops(gb, ops);

    Py_INCREF(Py_None);
    return Py_None;
}

/* Run a certain number of gpu frames.
   Currently a frame is simply a fixed number of cpu clocks. */
static PyObject *PyGameBoy_run_frames(PyGameBoy *self, PyObject *args) {
    unsigned int frames;
    if (!PyArg_ParseTuple(args, "i", &frames))
        return NULL;

    struct gameboy *gb = &(self->gb);
    gb_run_frames(gb, frames);

    Py_INCREF(Py_None);
    return Py_None;
}

/* Return the emulator core's pixel buffer in RGBA format. 
   This can be passed directly into an OpenGL texture. */
static PyObject *PyGameBoy_pixel_buffer(PyGameBoy *self, PyObject *args) {
    struct gameboy *gb = &(self->gb);
    uint32_t *pix = gb_pixel_buffer(gb);
    int buf_size = SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t);

    return Py_BuildValue("s#", pix, buf_size);
}

/* Pass key presses from your front end (whatever that may be), to the 
   emulator core. */
static PyObject *PyGameBoy_joypad_press(PyGameBoy *self, PyObject *args) {
    unsigned int key;
    if (!PyArg_ParseTuple(args, "i", &key))
        return NULL;

    struct gameboy *gb = &(self->gb);
    gb_joypad_press(gb, key);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PyGameBoy_joypad_release(PyGameBoy *self, PyObject *args) {
    unsigned int key;
    if (!PyArg_ParseTuple(args, "i", &key))
        return NULL;

    struct gameboy *gb = &(self->gb);
    gb_joypad_release(gb, key);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PyGameBoy_read_mem(PyGameBoy *self, PyObject *args) {
    uint16_t addr;
    if (!PyArg_ParseTuple(args, "i", &addr))
        return NULL;

    struct gameboy *gb = &(self->gb);
    uint8_t val = mem_read_u8(gb, addr);

    return Py_BuildValue("i", val);
}

static PyMemberDef PyGameBoy_members[] = {
    {"ime", T_UBYTE, offsetof(PyGameBoy, gb.ime), 0, "ime"},
    {"boot", T_INT, offsetof(PyGameBoy, gb.boot), 0, "boot"},
    {"rom_bank", T_INT, offsetof(PyGameBoy, gb.rom_bank), 0, "rom_bank"},
    {"ram_bank", T_INT, offsetof(PyGameBoy, gb.ram_bank), 0, "ram_bank"},
    {"AF", T_USHORT, offsetof(PyGameBoy, gb.cpu.AF), 0, "AF"},
    {"BC", T_USHORT, offsetof(PyGameBoy, gb.cpu.BC), 0, "BC"},
    {"DE", T_USHORT, offsetof(PyGameBoy, gb.cpu.DE), 0, "DE"},
    {"HL", T_USHORT, offsetof(PyGameBoy, gb.cpu.HL), 0, "HL"},
    {"PC", T_USHORT, offsetof(PyGameBoy, gb.cpu.PC), 0, "PC"},
    {"SP", T_USHORT, offsetof(PyGameBoy, gb.cpu.SP), 0, "SP"},
    {"A", T_UBYTE, offsetof(PyGameBoy, gb.cpu.A), 0, "A"},
    {"F", T_UBYTE, offsetof(PyGameBoy, gb.cpu.F), 0, "F"},
    {NULL}  /* Sentinel */
};

static PyMethodDef PyGameBoy_methods[] = {
    {"load_rom", (PyCFunction) PyGameBoy_load_rom, METH_VARARGS, NULL},
    {"run_ops", (PyCFunction) PyGameBoy_run_ops, METH_VARARGS, NULL},
    {"run_frames", (PyCFunction) PyGameBoy_run_frames, METH_VARARGS, NULL},
    {"pixel_buffer", (PyCFunction) PyGameBoy_pixel_buffer, METH_NOARGS, NULL},
    {"read_mem", (PyCFunction) PyGameBoy_read_mem, METH_VARARGS, NULL},
    {"joypad_press", (PyCFunction) PyGameBoy_joypad_press, METH_VARARGS, NULL},
    {"joypad_release", (PyCFunction) PyGameBoy_joypad_release, METH_VARARGS, NULL},
    {NULL}  /* Sentinel */
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

static PyTypeObject PyGameBoyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pygb.GameBoy",            /* tp_name */
    sizeof(PyGameBoy),         /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyGameBoy_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "GameBoy objects",         /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyGameBoy_methods,         /* tp_methods */
    PyGameBoy_members,         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyGameBoy_new,             /* tp_new */
};

PyMODINIT_FUNC initpygb(void) {
    PyObject *m;

    if (PyType_Ready(&PyGameBoyType) < 0)
        return;

    m = Py_InitModule("pygb", module_methods);
    if (m == NULL)
        return;

    Py_INCREF(&PyGameBoyType);
    PyModule_AddObject(m, "GameBoy", (PyObject *) &PyGameBoyType);
}
