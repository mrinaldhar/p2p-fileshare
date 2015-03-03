rm -f server
cc app.c mongoose.c -pthread -o app
gcc server.c mongoose.c -pthread -w -lssl -lcrypto -o server
