## 使用
```[shell]
mkdir build
cd build
cmake ..

[example-1]
g++ demo.cc -L{YOUR_DIR}/build/lib -lcnetStatic
g++ demo.cc -L{YOUR_DIR}/build/lib -lcnet
```

# 目录
```
├── demo
│   └── log
├── include
├── lib
├── src
│   └── http
├── test
│   └── data_test
└── update
```
