#pragma once

#define OBJ_SET(Obj, Var, Value) this->Loader->invoke("ObjCore", "Set", 3, Obj, Var, Value)
#define Obj_GET(Obj, Var, Value) this->Loader->invoke("ObjCore", "Get", 3, Obj, Var, Value)