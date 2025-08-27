.PHONY: run-dev


run-dev:
	cmake --preset=dev-linux .
	mv build/dev/compile_commands.json ./
	cmake --build build/dev
	@./build/dev/tcd $(PWD)

