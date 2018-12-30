#include<iostream>
#include<string>
#include<map>
#include<fstream>
#include<math.h>
#include<regex>
using namespace std;
regex hh(";");
string getstring(string inputstr)
{
string sout;
FILE * pFile;
long lSize;
char * buffer;
size_t result;
pFile = fopen ( inputstr.c_str() , "rb" );
if (pFile==NULL) {fputs ("找不到文件\n",stderr); exit (1);}
// obtain file size:
fseek (pFile , 0 , SEEK_END);
lSize = ftell (pFile);
rewind (pFile);
// allocate memory to contain the whole file:
buffer = (char*) malloc (sizeof(char)*lSize);
if (buffer == NULL) {fputs ("内存溢出\n",stderr); exit (2);}
// copy the file into the buffer:
result = fread (buffer,1,lSize,pFile);
if (result != lSize) {fputs ("文件读取错误\n",stderr); exit (3);}
/* the whole file is now loaded in the memory buffer. */
// terminate
sout.assign(buffer,result);
fclose (pFile);
free (buffer);
return sout;}
void writeFile(string str,string path){
	ofstream out(path);
	if(out.is_open()){
		out<<str;
		out.close();
	}
}
class data;
class data{
public:
	int integer;
	string str,type;
	double real;
	map<string,data> member;
	data *p;
	void *func;
	data(int in=0,string st="",string ty="NULL",double re=0){
		integer=in;
		str=st;
		type=ty;
		real=re;
	};
};
data vm(string code,map<string,data> domain=map<string,data>());
map<int,string> loopcondition,loops,ifcondition;
map<int,map<int,string> > loopstr;
map<int,int> looppos,end_pos,es_pos;
int l=-1,loopa=-1,la,ia=-1,ifa,esa=-1,arga;
bool isglobal=false;
map<string,data> global;
string num("0123456789"),symbol("+-*/&^.%=!|<>[]{}@:"),name;
string getunit(string str,int p){
	string onstr="";
	while(p<str.length()){
		p++;
		if(str[p]==' '){break;}
		onstr+=str[p];
	}
	return onstr;
}
bool isins(char text,string s){
	int i=0;
	while(i<s.length()){
		if(s[i]==text){
			return true;
		}
		i++;
	}
	return false;
}
data parse(string exp,map<string,data> &domain){
	data linshi;
	linshi.str=exp;
	if(exp[0]=='"'){
		if(exp.length()>2){
			linshi.str=regex_replace(exp.substr(1,exp.find('"',2)-1),hh,"\n");
		}
		linshi.type="str";
	}
	else if(exp.length()>0&&isins(exp[0],num)){
		linshi.type="float";
		linshi.real=stod(exp);
	}
	else if(exp=="true"){
		linshi.type="bool";
		linshi.integer=1;
	}
	else if(exp=="false"){
		linshi.type="bool";
		linshi.integer=0;
	}
	else if(exp=="NaN"){
		linshi.type="NaN";
	}
	else if(domain.find(exp)!=domain.end()){
		linshi=domain[exp];
		linshi.p=&domain[exp];
	}
	else if(global.find(exp)!=global.end()){
		linshi=global[exp];
		linshi.p=&global[exp];
	}
	return linshi;
}
string getdata(data obj){
	if(obj.type=="int"){
		obj.str=to_string(obj.integer);
	}
	else if(obj.type=="float"){
		if(obj.real==int(obj.real)){obj.str=to_string(int(obj.real));}
		else{
			obj.str=to_string(obj.real);
		}
	}
	else if(obj.type=="bool"){
		if(obj.integer==0){
			obj.str="false";
		}
		else{
			obj.str="true";
		}
	}
	else if(obj.type=="map"){
		obj.str=getdata(obj.member["0"])+":"+getdata(obj.member["1"]);
	}
	else if(obj.type=="list"){
		la=0;
		obj.str="";
		while(la<obj.integer){
			obj.str+=getdata(obj.member[to_string(la)])+",";
			la++;
		}
	}
	return obj.str;
}
data calculate(string exp,map<string,data> &domain){
	int i=0,d_a=-1,p=-1,p_list=-1;
	map<int,data> stack;
	map<int,int> pos,pos_list;
	data op2;
	string onstr(""),valuename;
	la=0;
	while(i<exp.length()){
		if(exp[i]=='"'){
			while(i<exp.length()){
				i++;
				if(exp[i]=='"'){
					break;
				}
				onstr+=exp[i];
			}
			onstr='"'+onstr+'"';
			d_a++;
			stack[d_a]=parse(onstr,domain);
			onstr="";
		}
		else if('0'<=exp[i]&&exp[i]<='9'){
			if(exp[i]<'0'||exp[i]>'9'){
				onstr+=exp[i];
			}
			else{
				onstr=exp[i];
			}
			while(i<exp.length()){
				i++;
				if(exp[i]==' 'or i==exp.length()-1){
					break;
				}
				onstr+=exp[i];
			}
			d_a++;
			stack[d_a]=parse(onstr,domain);
			onstr="";
		}
		else if(exp[i]==' ' or i==exp.length()-1){
			if(onstr.length()>0&&isins(onstr[0],symbol)){
				if(onstr=="{"){
					d_a++;
					stack[d_a]=data(0,"","dict");
					p++;
					pos[p]=d_a;
				}
				else if(onstr=="}"){
					while(d_a>pos[p]){
						stack[pos[p]].member[getdata(stack[d_a].member["0"])]=stack[d_a].member["1"];
						d_a--;
					}
					p--;
				}
				else if(onstr=="["){
					d_a++;
					stack[d_a]=data(0,"","list");
					p_list++;
					pos_list[p_list]=d_a;
				}
				else if(onstr=="]"){
					stack[pos_list[p_list]].integer=d_a-pos_list[p_list];
					while(d_a>pos_list[p_list]){
						stack[pos_list[p_list]].member[to_string(d_a-pos_list[p_list]-1)]=stack[d_a];
						d_a--;
					}
					p_list--;
				}
				else if(onstr=="@t"){
					stack[d_a]=data(0,stack[d_a].type,"str");
				}
				else if(onstr=="@ti"){
					stack[d_a].real=stoi(getdata(stack[d_a])),stack[d_a].type="float";
				}
				else if(onstr=="@ts"){
					stack[d_a].str=getdata(stack[d_a]),stack[d_a].type="str";
				}
				else if(onstr=="@td"){
					stack[d_a].real=stod(getdata(stack[d_a])),stack[d_a].type="float";
				}
				else if(onstr=="@i"){
					stack[d_a].real=int(stack[d_a].real);
				}
				else if(onstr=="@p"){
					stack[d_a].type="pt";
				}
				else{
					op2=stack[d_a],d_a--;
					if(onstr=="+"){
						if(stack[d_a].type=="str"){
							stack[d_a].str+=getdata(op2);
						}
						else if(stack[d_a].type!=op2.type){
							stack[0]=data(0,"","NaN");
							break;
						}
						else{
							stack[d_a].real+=op2.real;
						}
						
					}
					else if(onstr=="-"){
						if(stack[d_a].type!=op2.type){
							stack[0]=data(0,"","NaN");
							break;
						}
						else{
							stack[d_a].real-=op2.real;
						}
					}
					else if(onstr=="*"){
						if(stack[d_a].type!=op2.type){
							stack[0]=data(0,"","NaN");
							break;
						}
						else{
							stack[d_a].real*=op2.real;
							
						}
					}
					else if(onstr=="**"){
						if(stack[d_a].type!=op2.type){
							stack[0]=data(0,"","NaN");
							break;
						}
						else{
							stack[d_a].real=pow(stack[d_a].real,op2.real);
						}
					}
					else if(onstr=="/"){
						if(stack[d_a].type!=op2.type){
							stack[0]=data(0,"","NaN");
							break;
						}
						else{
							stack[d_a].real/=op2.real;
							
						}
					}
					else if(onstr=="%"){
						if(stack[d_a].type==op2.type){
							stack[d_a].real=stack[d_a].real-int(stack[d_a].real/op2.real)*op2.real;
							
						}
						else{
							stack[0]=data(0,"","NaN");
							break;
						}
					}
					else{
						if(onstr==">"){
							if(stack[d_a].type!="float"||stack[d_a].type!=op2.type){
								stack[d_a].integer=false;
							}
							else{
								stack[d_a].integer=stack[d_a].real>op2.real;
							}
							stack[d_a].type="bool";
							
						}
						else if(onstr=="<"){
							if(stack[d_a].type!="float"||stack[d_a].type!=op2.type){
								stack[d_a].integer=false;
							}
							else{
								stack[d_a].integer=stack[d_a].real<op2.real;
							}
							stack[d_a].type="bool";
							
						}
						else if(onstr=="=="){
							if(stack[d_a].type=="bool"&&op2.type=="bool"){
								stack[d_a].integer=stack[d_a].integer==op2.integer;
							}
							else{
								stack[d_a].str=getdata(stack[d_a]);
								op2.str=getdata(op2);
								stack[d_a].type="bool";
								stack[d_a].integer=stack[d_a].str==op2.str;
							}
						}
						else if(onstr=="<="){
							if(stack[d_a].type!="float"||stack[d_a].type!=op2.type){
								stack[d_a].integer=false;
							}
							else{
								stack[d_a].integer=stack[d_a].real<=op2.real;
							}
							stack[d_a].type="bool";
							
						}
						else if(onstr==">="){
							if(stack[d_a].type!="float"||stack[d_a].type!=op2.type){
								stack[d_a].integer=false;
							}
							else{
								stack[d_a].integer=stack[d_a].real>=op2.real;
							}
							stack[d_a].type="bool";
							
						}
						else if(onstr=="!="){
							if(stack[d_a].type=="bool"&&op2.type=="bool"){
								stack[d_a].integer=stack[d_a].integer!=op2.integer;
							}
							else{
								stack[d_a].str=getdata(stack[d_a]);
								op2.str=getdata(op2);
								stack[d_a].type="bool";
								stack[d_a].integer=stack[d_a].str!=op2.str;
							}
							
						}
						else if(onstr=="||"){
							if(stack[d_a].type!="bool"&&stack[d_a].type!=op2.type){
								stack[d_a].integer=false;
							}
							else{
								stack[d_a].integer=stack[d_a].integer||op2.integer;
							}
							stack[d_a].type="bool";
							
						}
						else if(onstr=="&&"){
							if(stack[d_a].type!="bool"&&stack[d_a].type!=op2.type){
								stack[d_a].integer=false;
							}
							else{
								stack[d_a].integer=stack[d_a].integer&&op2.integer;
							}
							stack[d_a].type="bool";
							
						}
						else{
							if(onstr==":"){
								stack[d_a].member["0"]=data(0,getdata(stack[d_a]),"str");
								stack[d_a].member["1"]=op2;
								
							}
							else if(onstr=="."){
								stack[d_a].p=&stack[d_a].p->member[getdata(op2)];
							}
							else if(onstr=="="){
								if(stack[d_a].type=="pt"){
									*stack[d_a].p=op2;
								}
								else if(isglobal){
									global[stack[d_a].str]=op2;
								}
								else{
									domain[stack[d_a].str]=op2;
								}
							}
							else if(onstr=="@"){
								stack[d_a]=stack[d_a].member[getdata(op2)];
							}
							else if(onstr=="@@"){
								la=0;
								while(la<stack[d_a].member["args"].integer){
									stack[d_a].member["args"].member[getdata(stack[d_a].member["args"].member[to_string(la)])]=op2.member[to_string(la)];
									la++;
								}
								stack[d_a]=vm(stack[d_a].member["code"].str,stack[d_a].member["args"].member);
								if(stack[d_a].type=="int"){
									stack[d_a].type="float";
									stack[d_a].real=stack[d_a].integer;
								}
							}
						}
					}
				}
			
			}
			else{
				if(onstr!=""){
					if(onstr[0]==0){i++,onstr="";continue;}
					d_a++;
					stack[d_a]=parse(onstr,domain);
				}
			}
			onstr="";
		}
		else{
			onstr+=exp[i];
		}
		i++;
	}
	if(stack[d_a].type=="float"){
		if(stack[d_a].real==int(stack[d_a].real)){
			stack[d_a].type="int",stack[d_a].integer=stack[d_a].real;
		}
	}
	return stack[0];
}
data vm(string code,map<string,data> domain){
	map<int,string> lines;
	int line=-1,i=0,line_x=0;
	string onstr="",valuename,pattern;
	if(code[code.length()-1]!=';'or code[code.length()-1]!='\n'){code+='\n';}
	while(i<code.length()){
		if(code[i]=='\n'){
			line++,lines[line]=onstr,onstr="";
			if(lines[line][lines[line].length()-1]!=' '){
				lines[line]+=' ';
			}
		}
		else if(code[i]=='	'){
			i++;
			continue;
		}
		else{
			onstr+=code[i];
		}
		i++;
	}
	if(onstr!=""){
		line++,lines[line]=onstr,onstr="";if(lines[line][lines[line].length()-1]!=' '){lines[line]+=' ';}
	}
	i=0,line++;
	while(i<line){
		while(line_x<lines[i].length()){
			if(lines[i][line_x]==' '){
				if(onstr[0]=='d'){
					if(onstr[1]=='g'){
						isglobal=true;
					}
					else{
						isglobal=false;
					}
					onstr="";
					while(lines[i].length()>line_x){
						line_x++;
						onstr+=lines[i][line_x];
					}
					calculate(onstr,domain);
				}
				else if(onstr=="so"){
					onstr="";
					while(line_x<lines[i].length()){
						line_x++;
						onstr+=lines[i][line_x];
					}
					if(onstr[onstr.length()-1]!=' '){onstr+=' ';}
					cout<<getdata(calculate(onstr,domain))<<"\n";
				}
				else if(onstr=="si"){
					cin>>domain["std"].str;
				}
				else if(onstr=="lp"){
					onstr="";
					while(lines[i].length()>line_x){
						line_x++;
						onstr+=lines[i][line_x];
					}
					l++;
					loopcondition[l]=onstr;
					loopa=-1,la=1;
					while(i<line){
						i++;
						valuename=lines[i].substr(0,3);
						if(valuename=="edl"){
							la--;
						}
						else if(valuename.substr(0,2)=="lp"){
							la++;
						}
						if(la==0){
							loopa++,loops[loopa]=lines[i];
							break;
						}
						loopa++,loops[loopa]=lines[i];
					}
					if(calculate(loopcondition[l],domain).integer){
						loopstr[l]=lines;
						looppos[l]=i;
						lines=loops;
						i=-1;
					}
					else{
						l--;
					}
				}
				else if(onstr=="cl"){
					valuename=getunit(lines[i],line_x);
					data linshi;
					line_x+=valuename.length()+1;
					if(global.find(valuename)!=global.end()){
						linshi=global[valuename];
					}
					else{
						linshi=domain[valuename];
					}
					arga=0;
					onstr="";
					while(line_x<lines[i].length()){
						line_x++;
						if(lines[i][line_x]==','){
							linshi.member[linshi.member["args"].member[to_string(arga)].str]=calculate(onstr,domain);
							arga++;
							onstr="";
						}
						else{
							onstr+=lines[i][line_x];
						}
					}
					vm(linshi.member["code"].str,linshi.member);
				}
				else if(onstr=="if"){
					onstr="";
					while(lines[i].length()>line_x){
						line_x++;
						onstr+=lines[i][line_x];
					}
					ia++;
					ifcondition[ia]=onstr;
					ifa=1;
					la=i;
					while(i<line){
						i++;
						valuename=lines[i].substr(0,3);
						onstr=valuename.substr(0,2);
						if(onstr=="if"){
							ifa++;
						}
						else if(onstr=="es"){
							es_pos[ia]=i;
							esa++;
						}
						else if(valuename=="edi"){
							ifa--;
						}
						if(ifa==0){
							break;
						}
					}
					if(calculate(ifcondition[ia],domain).integer){
						end_pos[ia]=i;
						i=la;
					}
					else{
						if(ia==esa){
							i=es_pos[ia];
						}
						else{
							ia--;
						}
					}
				}
				else if(onstr=="es"){
					i=end_pos[ia];
					esa--;
				}
				else if(onstr=="edl"){
					if(!calculate(loopcondition[l],domain).integer){
						i=looppos[l];
						lines=loopstr[l];
						l--;
					}
					else{
						i=-1;
					}
				}
				else if(onstr=="edi"){
					ia--;
				}
				else if(onstr=="fi"){
					onstr="";
					while(lines[i].length()>line_x){
						line_x++;
						onstr+=lines[i][line_x];
					}
					domain["std"].str=getstring(getdata(calculate(onstr,domain)));
					break;
				}
				else if(onstr=="fo"){
					onstr="";
					while(lines[i].length()>line_x){
						line_x++;
						onstr+=lines[i][line_x];
					}
					writeFile(domain["std"].str,getdata(calculate(onstr,domain)));
					break;
				}
				else if(onstr=="brk"){
					loopcondition[l]="false";
					i=looppos[l]-1;
				}
				else if(onstr=="rt"){
					onstr="";
					while(line_x<lines[i].length()){
						line_x++;
						onstr+=lines[i][line_x];
					}
					return calculate(onstr,domain);
				}
			}
			else{
				onstr+=lines[i][line_x];
			}
			line_x++;
		}
		line_x=0;
		onstr="";
		i++;
	}
	return data();
}

int main(int argc,char* argv[]){
	global["std"]=data(0,"","str");
	vm(getstring(argv[1]));
	return 0;
}