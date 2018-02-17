#pragma once

#define Send(ID, Stream, Size) this->Loader->invoke("IOCP", "Send", 3, ID, (char*)Stream, Size)