dbrun(){
    ~/workspace/Bytecask/build/src/core/core
}

dbclear(){
    rm  -r ~/workspace/Bytecask/db/*
}

dbrestart(){
    dbclear
    dbrun
}

rebuild(){
    rm -rf ~/workspace/Bytecask/build/*
    cmake -S ~/workspace/Bytecask -B ~/workspace/Bytecask/build
}