# TRAFT
Implement Raft Based C++17

# Build
```
# mkdir TRAFT/cmake/build
# cd TRAFT/cmake/build
```
**Linux**
`# cmake ../ -DCMAKE_INSTALL_PREFIX=./`
`# make`
`# make test`

**MAC**
`# cmake ../ -DCMAKE_INSTALL_PREFIX=./ -DOPENSSL_ROOT_DIR=/usr/local/Cellar/openssl@1.1/1.1.1g/ -DOPENSSL_LIBRARIES=/usr/local/Cellar/openssl@1.1/1.1.1g/lib`
`# make`
`# make test`
