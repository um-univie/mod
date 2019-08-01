#include "Term.hpp"

#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Term.hpp>
#include <mod/lib/Term/RawTerm.hpp>
#include <mod/lib/Term/WAM.hpp>

namespace mod {
namespace Term {

void mgu(const std::string &left, const std::string &right) {
	auto tRawLeftOpt = lib::IO::Term::Read::rawTerm(left, lib::Term::getStrings(), lib::IO::log());
	auto tRawRightOpt = lib::IO::Term::Read::rawTerm(right, lib::Term::getStrings(), lib::IO::log());
	if(!tRawLeftOpt || !tRawRightOpt) return;
	auto tRawLeft = tRawLeftOpt.get();
	auto tRawRight = tRawRightOpt.get();

	lib::Term::Wam machine, machineTemp;
	lib::Term::RawAppendStore storeMachine, storeMachineTemp;
	auto addrLeft = lib::Term::append(machine, tRawLeft, storeMachine);
	auto addrRight = lib::Term::append(machineTemp, tRawRight, storeMachineTemp);
	machine.setTemp(machineTemp);
	addrRight.type = lib::Term::AddressType::Temp;
	lib::IO::log() << "=================================================" << std::endl;
	lib::IO::Term::Write::wam(machine, lib::Term::getStrings(), lib::IO::log());
	lib::IO::log() << "Left = " << addrLeft << ", Right = " << addrRight << std::endl;
	lib::IO::log() << "Most general unifier of" << std::endl
			<< "\t";
	lib::IO::Term::Write::term(machine, addrLeft, lib::Term::getStrings(), lib::IO::log());
	lib::IO::log() << " =? ";
	lib::IO::Term::Write::term(machine, addrRight, lib::Term::getStrings(), lib::IO::log());
	lib::IO::log() << std::endl;
	lib::IO::log() << "\t";
	lib::IO::Term::Write::rawTerm(tRawLeft, lib::Term::getStrings(), lib::IO::log());
	lib::IO::log() << " =? ";
	lib::IO::Term::Write::rawTerm(tRawRight, lib::Term::getStrings(), lib::IO::log());
	lib::IO::log() << std::endl;
	
	auto mgu = machine.unifyHeapTemp(addrLeft.addr, addrRight.addr);
	lib::IO::Term::Write::wam(machine, lib::Term::getStrings(), lib::IO::log() << "is ");
	lib::IO::log() << "Left = " << machine.deref(addrLeft) << ", Right = " << machine.deref(addrRight) << std::endl;

	lib::IO::Term::Write::mgu(machine, mgu, lib::Term::getStrings(), lib::IO::log());
	lib::IO::log() << std::endl;
}

} // namespace Term
} // namespace mod