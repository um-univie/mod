#include "Term.hpp"

#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/ParsingError.hpp>
#include <mod/lib/IO/Term.hpp>
#include <mod/lib/Term/RawTerm.hpp>
#include <mod/lib/Term/WAM.hpp>

#include <iostream>

namespace mod::Term {

void mgu(const std::string &left, const std::string &right) {
	lib::Term::RawTerm tRawLeft, tRawRight;
	try {
		tRawLeft = lib::IO::Term::Read::rawTerm(left, lib::Term::getStrings());
		tRawRight = lib::IO::Term::Read::rawTerm(right, lib::Term::getStrings());
	} catch(const lib::IO::ParsingError &e) {
		std::cout << e.msg << '\n';
		return;
	}

	lib::Term::Wam machine, machineTemp;
	lib::Term::RawAppendStore storeMachine, storeMachineTemp;
	auto addrLeft = lib::Term::append(machine, tRawLeft, storeMachine);
	auto addrRight = lib::Term::append(machineTemp, tRawRight, storeMachineTemp);
	machine.setTemp(machineTemp);
	addrRight.type = lib::Term::AddressType::Temp;
	std::cout << "=================================================" << std::endl;
	lib::IO::Term::Write::wam(machine, lib::Term::getStrings(), std::cout);
	std::cout << "Left = " << addrLeft << ", Right = " << addrRight << std::endl;
	std::cout << "Most general unifier of" << std::endl
			<< "\t";
	lib::IO::Term::Write::term(machine, addrLeft, lib::Term::getStrings(), std::cout);
	std::cout << " =? ";
	lib::IO::Term::Write::term(machine, addrRight, lib::Term::getStrings(), std::cout);
	std::cout << std::endl;
	std::cout << "\t";
	lib::IO::Term::Write::rawTerm(tRawLeft, lib::Term::getStrings(), std::cout);
	std::cout << " =? ";
	lib::IO::Term::Write::rawTerm(tRawRight, lib::Term::getStrings(), std::cout);
	std::cout << std::endl;
	
	auto mgu = machine.unifyHeapTemp(addrLeft.addr, addrRight.addr);
	lib::IO::Term::Write::wam(machine, lib::Term::getStrings(), std::cout << "is ");
	std::cout << "Left = " << machine.deref(addrLeft) << ", Right = " << machine.deref(addrRight) << std::endl;

	lib::IO::Term::Write::mgu(machine, mgu, lib::Term::getStrings(), std::cout);
	std::cout << std::endl;
}

} // namespace mod::Term