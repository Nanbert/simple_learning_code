VPATH = code
#vpath %.H code
#vpath %.cpp code
excelToXml: ele.H excelToXml.H main.cpp readConfig.H
		g++ -o excelToXml -c $< -ltinyxml2 -lxlnt -lconfig++ 
clean:
	rm excelToXml
