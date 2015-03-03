rm -f server
clear
echo
echo
echo "~~~~~~~~~~~~ mSquared File Transfer ~~~~~~~~~~~~"
echo "Mrinal Dhar ( 201325118 )"
echo "Mounika Somisetty ( 201330076 )"
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

echo "Make sure you have OpenSSL installed for MD5 to work properly. "
echo 
echo
gcc server.c mongoose.c -pthread -w -lssl -lcrypto -o server
