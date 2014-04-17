
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

#include "common-classes.hh"
#include "user-options.hh"
#include "error-display.hh"
#include "local-environment.hh"
#include "icode.hh"
#include "reg-alloc.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "program.hh"

Machine_Description machine_dscr_object;

//////////////////////////// Register Descriptor ///////////////////////////////

Register_Descriptor::Register_Descriptor(Spim_Register reg, string s, Register_Val_Type vt, Register_Use_Category uc)
{
	reg_id = reg;
	reg_name = s;
	value_type = vt;
	reg_use = uc; 
	used_for_expr_result = false;
}

Register_Val_Type Register_Descriptor::get_val_type(){ return value_type;}
Register_Use_Category Register_Descriptor::get_use_category() 	{ return reg_use; }
Spim_Register Register_Descriptor::get_register()             	{ return reg_id; }
string Register_Descriptor::get_name()				{ return reg_name; }
bool Register_Descriptor::is_symbol_list_empty()         	{ return lra_symbol_list.empty(); }

bool Register_Descriptor::is_free()     
{ 
	if ((reg_use == gp_data) && (lra_symbol_list.empty()) && !used_for_expr_result) 
		return true;
	else
		return false;
}

void Register_Descriptor::remove_symbol_entry_from_list(Symbol_Table_Entry & sym_entry)
{
	lra_symbol_list.remove(&sym_entry);
}

bool Register_Descriptor::find_symbol_entry_in_list(Symbol_Table_Entry & sym_entry)
{
	list<Symbol_Table_Entry *>::iterator i;
	for (i = lra_symbol_list.begin(); i != lra_symbol_list.end(); i++)
		if (**i == sym_entry)
			return true;

	return false;
}

void Register_Descriptor::clear_lra_symbol_list()
{
	list<Symbol_Table_Entry *>::iterator i;
	for (i = lra_symbol_list.begin(); i != lra_symbol_list.end(); i++)
	{
		Symbol_Table_Entry & sym_entry = **i;
		sym_entry.set_register(NULL);
	}

	lra_symbol_list.clear();
}

void Register_Descriptor::update_symbol_information(Symbol_Table_Entry & sym_entry)
{
	// std::cout << &sym_entry << endl;
	if (find_symbol_entry_in_list(sym_entry) == false)
		lra_symbol_list.push_back(&sym_entry);
}

void Register_Descriptor::reset_use_for_expr_result(bool used){
	used_for_expr_result = used;
}

bool Register_Descriptor::get_used_for_expr_result(){
	return used_for_expr_result;
}

int Register_Descriptor::get_lra_table_size() { return lra_symbol_list.size(); }

list<Symbol_Table_Entry *> & Register_Descriptor::get_lra_table()
{
	return lra_symbol_list;
}


//////////////////////////////// Lra_Outcome //////////////////////////////////////////

Lra_Outcome::Lra_Outcome(Register_Descriptor * rdp, bool nr, bool sr, bool dr, bool mv, bool ld)
{
	register_description = rdp;
	is_a_new_register = nr;
	is_same_as_source = sr;
	is_same_as_destination = dr;
	register_move_needed = mv; 
	load_needed = ld;
}

Lra_Outcome::Lra_Outcome()
{
	register_description = NULL;
	is_a_new_register = false;
	is_same_as_source = false;
	is_same_as_destination = false;
	register_move_needed = false;
	load_needed = false;
}

Register_Descriptor * Lra_Outcome::get_register() 	{ return register_description; }
bool Lra_Outcome::is_new_register()      		{ return is_a_new_register; }
bool Lra_Outcome::is_source_register()   		{ return is_same_as_source; }
bool Lra_Outcome::is_destination_register()    		{ return is_same_as_destination; }
bool Lra_Outcome::is_move_needed()      		{ return register_move_needed; }
bool Lra_Outcome::is_load_needed()	     		{ return load_needed; }

void Lra_Outcome::optimize_lra(Lra_Scenario lcase, Ast * destination_memory, Ast * source_memory)
{
	// Register allocation is done only when the source is in either memory or is a constant

	Register_Descriptor * destination_register, * source_register, * result_register;
	Symbol_Table_Entry * source_symbol_entry, * destination_symbol_entry;

	destination_register = NULL;
	source_register = NULL;
	result_register = NULL;

	is_a_new_register = false;
	is_same_as_source = false;
	is_same_as_destination = false;
	register_move_needed = false;
	load_needed = false;

	destination_symbol_entry = NULL;

	switch (lcase)
	{
	case mc_2m:
		CHECK_INVARIANT(destination_memory, 
			"Destination ast pointer cannot be NULL for m2m scenario in lra");
		CHECK_INVARIANT(source_memory, 
			"Source ast pointer cannot be NULL for m2m scenario in lra");

		// typeid usage: http://stackoverflow.com/a/1986485/1504267
		if (typeid(*destination_memory) == typeid(Number_Ast<int>) || typeid(*destination_memory) == typeid(Number_Ast<float>))
			destination_register = NULL;
		else
		{
			destination_symbol_entry = &(destination_memory->get_symbol_entry());
			destination_register = destination_symbol_entry->get_register(); 
		}

		if (typeid(*source_memory) == typeid(Number_Ast<int>) || typeid(*source_memory) == typeid(Number_Ast<float>))
			source_register = NULL;
		else
		{
			source_symbol_entry = &(source_memory->get_symbol_entry());
			source_register = source_symbol_entry->get_register(); 
		}

		if (source_register != NULL)
		{
			result_register = source_register;
			is_same_as_source = true;
			load_needed = false;
		}
		// do not set the result register to destination_register if destination_register
		// is already holding the value for multiple symbol table entries
		else if (destination_register != NULL and destination_register->get_lra_table_size() <= 1)
		{

			result_register = destination_register;
			is_same_as_destination = true;
			load_needed = true;
		}
		else 
		{
			// change type
			// cout<<"idhar aaya"<<endl;
			if(destination_memory->get_data_type() == int_data_type){
				result_register = machine_dscr_object.get_new_register(0);
			}
			else{
				result_register = machine_dscr_object.get_new_register(1);	
			}
			is_a_new_register = true;
			load_needed = true;
		}

		break;

	case mc_2r:
		CHECK_INVARIANT(source_memory, "Source ast pointer cannot be NULL for m2r scenario in lra");

		source_symbol_entry = &(source_memory->get_symbol_entry());
		source_register = source_symbol_entry->get_register(); 

		if (source_register != NULL)
		{
			result_register = source_register;
			is_same_as_source = true;
			load_needed = false;
		}
		else 
		{
			// change type
			if(source_memory->get_data_type() == int_data_type){
				result_register = machine_dscr_object.get_new_register(0);
			}
			else{
				result_register = machine_dscr_object.get_new_register(1);	
			}
			is_a_new_register = true;
			load_needed = true;
		}

		break;

	case r2r:
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
				"r2r scenario does not call for local register allocation");

		break;	

	case r2m:
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
				"r2m scenario does not call for local register allocation. Register allocation only allowed in mc_2m or mc_2r. So register allocation is not required in r2m");

		break;

	case c2r:
		//change type
		if(source_memory->get_data_type() == int_data_type){
			result_register = machine_dscr_object.get_new_register(0);
		}
		else{
			result_register = machine_dscr_object.get_new_register(1);	
		}
		is_a_new_register = true;
		load_needed = true;

		break;

	default:
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
				"Illegal local register allocation scenario");

		break;
	}

	CHECK_INVARIANT ((result_register != NULL), "Inconsistent information in lra");
	register_description = result_register;

	if (destination_symbol_entry != NULL && destination_register != NULL) {
		destination_symbol_entry->free_register(destination_register);
	}

	if (destination_symbol_entry) {
		destination_symbol_entry->update_register(result_register);
	}
}

/******************************* Machine Description *****************************************/

void Machine_Description::initialize_register_table()
{
	spim_register_table[zero] = new Register_Descriptor(zero, "zero", int_num, fixed_reg);
	spim_register_table[v0] = new Register_Descriptor(v0, "v0", int_num, gp_data);
	spim_register_table[v1] = new Register_Descriptor(v1, "v1", int_num, fn_result);
	spim_register_table[a0] = new Register_Descriptor(a0, "a0", int_num, argument);
	spim_register_table[a1] = new Register_Descriptor(a1, "a1", int_num, argument);
	spim_register_table[a2] = new Register_Descriptor(a2, "a2", int_num, argument);
	spim_register_table[a3] = new Register_Descriptor(a3, "a3", int_num, argument);
	spim_register_table[t0] = new Register_Descriptor(t0, "t0", int_num, gp_data);
	spim_register_table[t1] = new Register_Descriptor(t1, "t1", int_num, gp_data);
	spim_register_table[t2] = new Register_Descriptor(t2, "t2", int_num, gp_data);
	spim_register_table[t3] = new Register_Descriptor(t3, "t3", int_num, gp_data);
	spim_register_table[t4] = new Register_Descriptor(t4, "t4", int_num, gp_data);
	spim_register_table[t5] = new Register_Descriptor(t5, "t5", int_num, gp_data);
	spim_register_table[t6] = new Register_Descriptor(t6, "t6", int_num, gp_data);
	spim_register_table[t7] = new Register_Descriptor(t7, "t7", int_num, gp_data);
	spim_register_table[t8] = new Register_Descriptor(t8, "t8", int_num, gp_data);
	spim_register_table[t9] = new Register_Descriptor(t9, "t9", int_num, gp_data);
	spim_register_table[s0] = new Register_Descriptor(s0, "s0", int_num, gp_data);
	spim_register_table[s1] = new Register_Descriptor(s1, "s1", int_num, gp_data);
	spim_register_table[s2] = new Register_Descriptor(s2, "s2", int_num, gp_data);
	spim_register_table[s3] = new Register_Descriptor(s3, "s3", int_num, gp_data);
	spim_register_table[s4] = new Register_Descriptor(s4, "s4", int_num, gp_data);
	spim_register_table[s5] = new Register_Descriptor(s5, "s5", int_num, gp_data);
	spim_register_table[s6] = new Register_Descriptor(s6, "s6", int_num, gp_data);
	spim_register_table[s7] = new Register_Descriptor(s7, "s7", int_num, gp_data);
	spim_register_table[gp] = new Register_Descriptor(gp, "gp", int_num, pointer);
	spim_register_table[sp] = new Register_Descriptor(sp, "sp", int_num, pointer);
	spim_register_table[fp] = new Register_Descriptor(fp, "fp", int_num, pointer);
	spim_register_table[ra] = new Register_Descriptor(ra, "ra", int_num, ret_address);

	spim_register_table[f0] = new Register_Descriptor(f0, "f0", float_num, fn_result);
	spim_register_table[f2] = new Register_Descriptor(f2, "f2", float_num, gp_data);
	spim_register_table[f4] = new Register_Descriptor(f4, "f4", float_num, gp_data);
	spim_register_table[f6] = new Register_Descriptor(f6, "f6", float_num, gp_data);
	spim_register_table[f8] = new Register_Descriptor(f8, "f8", float_num, gp_data);
	spim_register_table[f10] = new Register_Descriptor(f10, "f10", float_num, gp_data);
	spim_register_table[f12] = new Register_Descriptor(f12, "f12", float_num, gp_data);
	spim_register_table[f14] = new Register_Descriptor(f14, "f14", float_num, gp_data);
	spim_register_table[f16] = new Register_Descriptor(f16, "f16", float_num, gp_data);
	spim_register_table[f18] = new Register_Descriptor(f18, "f18", float_num, gp_data);
	spim_register_table[f20] = new Register_Descriptor(f20, "f20", float_num, gp_data);
	spim_register_table[f22] = new Register_Descriptor(f22, "f22", float_num, gp_data);
	spim_register_table[f24] = new Register_Descriptor(f24, "f24", float_num, gp_data);
	spim_register_table[f26] = new Register_Descriptor(f26, "f26", float_num, gp_data);
	spim_register_table[f28] = new Register_Descriptor(f28, "f28", float_num, gp_data);
	spim_register_table[f30] = new Register_Descriptor(f30, "f30", float_num, gp_data);
}

void Machine_Description::initialize_instruction_table()
{
	spim_instruction_table[store] = new Instruction_Descriptor(store, "store", "sw", "", i_r_op_o1, a_op_o1_r);
	spim_instruction_table[neg] = new Instruction_Descriptor(neg, "uminus", "neg", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[load] = new Instruction_Descriptor(load, "load", "lw", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[imm_load] = new Instruction_Descriptor(imm_load, "iLoad", "li", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[sgt] = new Instruction_Descriptor(sgt, "sgt", "sgt", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[slt] = new Instruction_Descriptor(slt, "slt", "slt", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[sle] = new Instruction_Descriptor(sle, "sle", "sle", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[sge] = new Instruction_Descriptor(sge, "sge", "sge", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[seq] = new Instruction_Descriptor(seq, "seq", "seq", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[sne] = new Instruction_Descriptor(sne, "sne", "sne", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[bne] = new Instruction_Descriptor(bne, "bne", "bne", "", i_op_o1_col_o2, a_op_o1_col_o2);
	spim_instruction_table[_goto] = new Instruction_Descriptor(_goto, "goto", "j", "", i_op_o1, a_op_o1);
	
	spim_instruction_table[stored] = new Instruction_Descriptor(stored, "store.d", "s.d", "", i_r_op_o1, a_op_o1_r);
	spim_instruction_table[loadd] = new Instruction_Descriptor(loadd, "load.d", "l.d", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[imm_loadd] = new Instruction_Descriptor(imm_loadd, "iLoad.d", "li.d", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[mfc1] = new Instruction_Descriptor(mfc1, "mfc1", "mfc1", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[mtc1] = new Instruction_Descriptor(mtc1, "mtc1", "mtc1", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[negd] = new Instruction_Descriptor(negd, "uminus.d", "neg.d", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[moved] = new Instruction_Descriptor(moved, "move.d", "move.d", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[_move] = new Instruction_Descriptor(_move, "move", "move", "", i_r_op_o1, a_op_r_o1);
	spim_instruction_table[move_ret] = new Instruction_Descriptor(move_ret, "move", "move", "", i_op_r_o1, a_op_r_o1);
	spim_instruction_table[move_ret_d] = new Instruction_Descriptor(move_ret_d, "move.d", "move.d", "", i_op_r_o1, a_op_r_o1);

	spim_instruction_table[add] = new Instruction_Descriptor(add, "add", "add", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[add_s] = new Instruction_Descriptor(add_s, "add", "add", "", i_nsy, a_op_r_o1_o2);
	spim_instruction_table[sub] = new Instruction_Descriptor(sub, "sub", "sub", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[sub_s] = new Instruction_Descriptor(sub_s, "sub", "sub", "", i_nsy, a_op_r_o1_o2);
	spim_instruction_table[mul] = new Instruction_Descriptor(mul, "mul", "mul", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[_div] = new Instruction_Descriptor(_div, "div", "div", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[addd] = new Instruction_Descriptor(addd, "add.d", "add.d", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[subd] = new Instruction_Descriptor(subd, "sub.d", "sub.d", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[muld] = new Instruction_Descriptor(muld, "mul.d", "mul.d", "", i_r_o1_op_o2, a_op_r_o1_o2);
	spim_instruction_table[_divd] = new Instruction_Descriptor(_divd, "div.d", "div.d", "", i_r_o1_op_o2, a_op_r_o1_o2);
	
	spim_instruction_table[call] = new Instruction_Descriptor(call, "call", "jal", "", i_op_o1, a_op_o1);
	// spim_instruction_table[j] = new Instruction_Descriptor(call, "call", "jal", "", i_op_o1, a_op_o1);
	spim_instruction_table[ret] = new Instruction_Descriptor(ret, "return", "j", "", i_op, a_op_o1);
}

void Machine_Description::validate_init_local_register_mapping()
{
	map<Spim_Register, Register_Descriptor *>::iterator i;
	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		Register_Descriptor * reg_desc = i->second;
		if(reg_desc->get_use_category() == gp_data && !reg_desc->is_free()){
			cout<<"REGISTER NUMBER "<<i->first<<endl;
		}
		if (reg_desc->get_use_category() == gp_data){
			CHECK_INVARIANT(reg_desc->is_free(), 
					"GP data registers should be free at the start of a basic block");
		}
	}
}

void Machine_Description::clear_local_register_mappings()
{
	map<Spim_Register, Register_Descriptor *>::iterator i;
	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		Register_Descriptor * reg_desc = i->second;
		reg_desc->clear_lra_symbol_list();
		reg_desc->reset_use_for_expr_result(false);
	}

	/* 
	Note that we do not need to save values at the end
	of a basic block because the values have already been
	saved for each assignment statement. Any optimization
	that tries to postpone the store statements may have to 
	consider storing all unstored values at the end of
	a basic block.
	*/
}

Register_Descriptor * Machine_Description::get_new_register(int val_type)
{
	Register_Descriptor * reg_desc;

	map<Spim_Register, Register_Descriptor *>::iterator i;
	for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
	{
		reg_desc = i->second;

		if (reg_desc->is_free() && reg_desc->get_val_type() == val_type)
			return reg_desc;
	}

	// if lra is on, it tries to remember registers
	// so see if some previous register can be freed
	if (command_options.is_do_lra_selected() == true) {
		for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
		{
			reg_desc = i->second;

			if (reg_desc->get_use_category() == gp_data and reg_desc->get_val_type() == val_type and !reg_desc->get_used_for_expr_result())
			{
				list<Symbol_Table_Entry *>::iterator it;
				it = reg_desc->get_lra_table().begin();
				(*it)->free_register(reg_desc);
				// clearing all the registers
				// return reg_desc;
			}
		}

		for (i = spim_register_table.begin(); i != spim_register_table.end(); i++)
		{
			reg_desc = i->second;

			if (reg_desc->is_free() && reg_desc->get_val_type() == val_type)
				return reg_desc;
		}
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
			"Error in get_new_reg or register requirements of input program cannot be met");
}
