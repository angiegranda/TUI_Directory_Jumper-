.PHONY: run-dev run-dev-windows


run-dev:
	cmake --preset=dev-linux .
	mv build/dev/compile_commands.json ./
	cmake --build build/dev
	@./build/dev/tcd $(PWD)

run-dev-windows:
	cmake --preset=dev-windows .
	cmake --build build/dev
	@build\dev\tcd.exe %CD%

