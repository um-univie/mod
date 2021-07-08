#include "RawTerm.hpp"
#include "WAM.hpp"

#include <mod/Config.hpp>
#include <mod/Error.hpp>
#include <mod/lib/IO/IO.hpp>

#include <unordered_map>

namespace mod::lib::Term {

Address append(Wam &machine, const RawTerm &term, RawAppendStore &varToAddr) {
	struct Writer {
		Writer(Wam &machine, RawAppendStore &varToAddr) : machine(machine), varToAddr(varToAddr) {}

		//		std::pair<Cell, bool> operator()(RawVariable var) const {
		//			auto iter = varToAddr.find(var.name);
		//			if(iter == end(varToAddr)) {
		//				return std::make_pair(Cell{CellTag::REF, 0, 0}, true);
		//			} else {
		//				auto addr = machine.putRefPtr();
		//				machine.heap[addr].value = iter->second;
		//				return std::make_pair(Cell{CellTag::REF, 0, iter->second}, false);
		//			}
		//		}

		std::size_t operator()(const RawStructure &str) const {
			// recurse and save pointers
			std::vector<std::size_t> strPtrs;
			for(const auto &term : str.args) {
				if(const RawStructure *subStr = std::get_if<RawStructure>(&term)) {
					// space optimization: constants are written in-line
					if(!subStr->args.empty()) {
						strPtrs.push_back((*this)(*subStr));
					}
				}
			}
			auto strAddr = machine.putStructure(str.name, str.args.size());
			std::size_t nextStrPtr = 0;
			for(auto term : str.args) {
				if(const RawVariable *var = std::get_if<RawVariable>(&term)) {
					const auto iter = varToAddr.find(var->name);
					if(iter == end(varToAddr)) {
						auto vAddr = machine.putRefPtr();
						varToAddr.emplace(var->name, vAddr);
					} else {
						auto addr = machine.putRefPtr();
						machine.getCell(addr).REF.addr = iter->second;
					}
				} else {
					const RawStructure &str = std::get<RawStructure>(term);
					if(str.args.empty()) {
						// space optimization: constants are written in-line
						(*this)(str);
					} else {
						assert(nextStrPtr < strPtrs.size());
						machine.putStructurePtr(strPtrs[nextStrPtr]);
						nextStrPtr++;
					}
				}
			}
			assert(nextStrPtr == strPtrs.size());
			return strAddr.addr;
		}
	private:
		Wam &machine;
		std::unordered_map<std::size_t, Address> &varToAddr;
	};

	if(const RawStructure *str = std::get_if<RawStructure>(&term)) {
		return Address{AddressType::Heap, Writer(machine, varToAddr)(*str)};
	} else if(const RawVariable *var = std::get_if<RawVariable>(&term)) {
		const auto iter = varToAddr.find(var->name);
		if(iter == end(varToAddr)) {
			auto addr = machine.putRefPtr();
			varToAddr.emplace(var->name, addr);
			return addr;
		} else {
			return iter->second;
		}
	} else
		MOD_ABORT;
}

} // namespace mod::lib::Term