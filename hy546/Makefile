helloworld:
	@clang -S -emit-llvm  helloworld.c 
	@../build/bin/opt ./helloworld.ll -passes=printfpass |../build/bin/lli

printsource:
	@clang -S -emit-llvm  printsource.c
	@../build/bin/opt ./printsource.ll -passes=printfpass |../build/bin/lli



clean:
	rm -rf *.ll log.txt
