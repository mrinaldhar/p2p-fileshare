rm -f server
cc app.c mongoose.c -pthread -o app
gcc server.c -pthread -w -o server
