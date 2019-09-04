#pragma once
#include "common.h"
#include "value.h"
#include "gc.h"
namespace lunatic {
	class Value;
	class Table;
	class VM;
	class Table : public GCObject {
		std::unordered_map<std::string, Value> sMap;
		std::unordered_map<int, Value> iMap;
		std::vector<Value> list;
	public:
		friend class Value;

		Value get(int);

		Value get(const std::string&);

		void set(int, const Value&);

		void set(const std::string&, const Value&);

		Table() : sMap(), iMap() { list.push_back(Value()); }

		inline int len() const { return list.size() - 1; }

		inline std::vector<Value>& getList() { return list; }
		inline bool isNil()const {
			return len() == 0 && sMap.size() == 0;
		}
		void markReferences(GC*)const override;
		virtual size_t nBytes()const{
			return sizeof(*this);
		}
		struct end_tag_t {};
		class iterator {
			Table* table;
			enum State {
				init,
				list,
				iMap,
				sMap,
				end
			};
			State state = init;
			int listIter;
			std::unordered_map<std::string, Value>::iterator sMapIter;
			std::unordered_map<int, Value>::iterator iMapIter;
		public:
			void next() {
				if (state == init) {
					listIter = 0;
					state = list;
					if (listIter >= table->list.size()) {
						state = iMap;
						iMapIter = table->iMap.begin();
						if (iMapIter == table->iMap.end()) {
							state = sMap;
							sMapIter = table->sMap.begin();
							if (sMapIter == table->sMap.end()) {
								state = end;
							}
						}
					}
				}
				else if (state == list) {
					listIter++;
					if (listIter >= table->list.size()) {
						state = iMap;
						iMapIter = table->iMap.begin();
						if (iMapIter == table->iMap.end()) {
							state = sMap;
							sMapIter = table->sMap.begin();
							if (sMapIter == table->sMap.end()) {
								state = end;
							}
						}
					}
				}
				else if (state == iMap) {
					iMapIter++;
					if (iMapIter == table->iMap.end()) {
						state = sMap;
						sMapIter = table->sMap.begin();
						if (sMapIter == table->sMap.end()) {
							state = end;
						}
					}
				}
				else if (state == sMap) {
					sMapIter++;
					if (sMapIter == table->sMap.end()) {
						state = end;
					}
				}
			}
		
			iterator(Table* t) :table(t) {
				next();
			}
			iterator(Table* t, end_tag_t):state(end), table(t) {}
			bool operator == (const iterator& iter)const {
				if (iter.state != state|| iter.table != table)return false;
				if (state == init||state == end) {
					return true;
				}
				if (state == list) {
					return listIter == iter.listIter;
				}
				if (state == iMap) {
					return iMapIter == iter.iMapIter;
				}
				return sMapIter == iter.sMapIter;
			}
			std::pair<Value, Value> get(VM* vm)const;
		};
		iterator begin() {
			return iterator(this);
		}
		iterator end() {
			return iterator(this, end_tag_t());
		}		
	};
}
