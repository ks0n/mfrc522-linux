obj-m += mfrc522full.o
mfrc522full-objs += mfrc522.o

rust_src=$(find src -name "*.rs")

MAKE = make -C /lib/modules/$(shell uname -r)/build M=$(PWD)

all: kbuild_init rust
	$(MAKE) modules

kbuild_init:
	$(MAKE) modules || true # We now this will fail but we need *full.o

rust:
	cargo build #--target=armv7-unknown-linux-musleabihf
	ld -r mfrc522full.o target/debug/libmfrc522_rs.a -o mfrc522.o # Warning allways put all the .o files before any .a

clean:
	cargo clean
	$(MAKE) clean
