all:
	cd proto_lib;make;cd ../
	cd utils;make;cd ../
	cd config;make;cd ../
	cd account_common;make;cd ../
	cd account_open;make;cd ../

cgi:
	cd utils;make;cd ../
	cd config;make;cd ../
	cd account_common;make CGI=1;cd ../

clean:
	cd proto_lib;make clean;cd ../
	cd utils;make clean;cd ../
	cd config;make clean;cd ../
	cd account_common;make clean;cd ../
	cd account_open;make clean;cd ../

cleancgi:
	cd utils;make clean;cd ../
	cd config;make clean;cd ../
	cd account_common;make clean;cd ../
