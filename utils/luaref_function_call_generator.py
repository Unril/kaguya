
def generate_args(out,arg_num,name):
	if arg_num > 0:
		out.write(name + '1')
		for i in range (1,arg_num):
			out.write(','+name + str(i+1))

def generate_template(out,arg_num):
	if arg_num > 0:
		out.write('template<')
		generate_args(out,arg_num,'typename T')
		out.write('>\n')

def generate_fun_args(out,arg_num):
	for i in range (arg_num):
		if i != 0:
			out.write(',')
		out.write('T' + str(i+1)+' t' + str(i+1))

def generate(out,arg_num):
	generate_template(out,arg_num)
	out.write('inline FunEvaluator LuaRef::operator()(')
	generate_fun_args(out,arg_num)
	out.write(')\n')
	out.write('{\n')
	out.write('  value_type typ = type();\n')
	out.write('  if(typ != TYPE_FUNCTION && typ != TYPE_THREAD){except::typeMismatchError(state_, "is not function");return FunEvaluator(state_);}\n')
	out.write('  int argstart = lua_gettop(state_);\n')
	for i in range (1,arg_num+1):
		out.write('  types::push_dispatch(state_,standard::forward<T' +  str(i) + '>(t' + str(i) + '));\n')
	out.write('  int argnum = lua_gettop(state_) - argstart;\n')
	out.write('  std::vector<LuaRef> args;\n')
	out.write('  args.reserve(argnum);\n')

	out.write('  for (int i = 0; i < argnum; ++i)\n')
	out.write('    args.push_back(LuaRef(state_, StackTop()));\n')
	out.write('  std::reverse(args.begin(), args.end());\n')

	out.write('  return FunEvaluator(state_,*this,args);\n')
	out.write('}\n')

if __name__ == "__main__":
	import sys
	sys.stdout.write('//generated header by ' + __file__ + "\n")
	for i in range(10):
		generate(sys.stdout,i)
