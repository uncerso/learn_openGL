release:
	CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake -B ./build -DCMAKE_BUILD_TYPE=Release -G Ninja
	cmake --build ./build --config Release --parallel

clean:
	rm -rf ./build
