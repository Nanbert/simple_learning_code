void readConfig(Config &cfg)
{
	try
	{
		cfg.readFile("/home/nanbert/box/csToCsharp.cfg");
	}
	catch(const FileIOException &fioex)
	{
		cerr<<"I/O error while reading file."<<endl;
		exit;
	}
	catch(const ParseException &pex)
	{
		cerr<<"Parse error at"<<pex.getFile()<<":"<<pex.getLine()
			<<"-"<<pex.getError()<<endl;
		exit;
	}
	return ;
}
