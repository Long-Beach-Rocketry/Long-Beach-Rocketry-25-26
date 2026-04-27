## Building and Generating Code with nanopb

### Prerequisites

- Install Python (recommended: use a virtual environment)
- Install Protocol Buffers compiler (`protoc`)
- Install the required protobuf Python package:
  
	```sh
	pip install --upgrade protobuf
	# or for a specific version:
	pip install protobuf==4.21.12
	```

- Check your `protoc` version:
  
	```sh
	protoc --version
	```

### Generating C Code from .proto

Use the following command to generate nanopb C files from your .proto file running the: msg.ps1 shell script

Replace `protobuff.proto` with your .proto file as needed. (well we anemd it this way no need to change)

---
## What you need 

` set CC=gcc
set CFLAGS=-O2
set PROTOC=C:\path\to\protoc.exe
set PYTHON=python`
