//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"

namespace Luxko {
	class bad_optional_access : public std::logic_error {
	public:
		explicit bad_optional_access(const char* msg) : std::logic_error(msg) {}
		explicit bad_optional_access(const std::string& msg) : std::logic_error(msg) {}
		virtual ~bad_optional_access() {}
	};

	class nullopt_t {};

	nullopt_t nullopt;

	class in_place_tag {};

	class in_place_param {};

	template<class T> class in_place_type_param {};

	template<size_t I> class in_place_index_param {};


	in_place_tag in_place(in_place_param) { return in_place_tag(); }

	template<class T>
	in_place_tag in_place(in_place_type_param<T>) { return in_place_tag(); }

	template<size_t I>
	in_place_tag in_place(in_place_index_param<I>) { return in_place_tag(); }

	using in_place_t = in_place_tag(&)(in_place_param);

	template<class T>
	using in_place_type_t = in_place_tag(&)(in_place_type_param<T>);

	template<size_t I>
	using in_place_index_t = in_place_tag(&)(in_place_index_param<I>);


	template<class T> // T must be Destructible
	class Optional {
	public:
		Optional() {}
		Optional(nullopt_t) {}

		Optional(const Optional& other) {
			if (other) {
				_valuePtr = new(_data) T(*other);
			}
		}
		Optional(Optional&& other) {
			if (other) {
				_valuePtr = new(_data) T(*other);
			}
		}

		Optional(const T& value) {
			_valuePtr = new(_data) T(value);
		}
		Optional(T&& value) {
			_valuePtr = new(_data) T(value);
		}

		template<class ... Args>
		explicit Optional(in_place_t, Args&&... args) {
			_valuePtr = new(_data) T(args...);
		}

		~Optional() {
			reset();
		}

	public: // Queries

		Optional& operator=(nullopt_t) { reset();  return *this; }
		Optional& operator=(const Optional& other) {
			reset();
			if (other) {
				_valuePtr = new(_data) T(*other);
			}
			return *this;
		}
		Optional& operator=(Optional&& other) {
			reset();
			if (other) {
				_valuePtr = new(_data) T(*other);
			}
			return *this;
		}
		template<class U> Optional& operator=(U&& value) {
			reset();
			_valuePtr = new(_data) T(value);
			return *this;
		}

		// The following methods does not check if the optional contains any value.
		const T* operator->()const noexcept {
			return _valuePtr;
		}
		T* operator->()noexcept {
			return _valuePtr;
		}
		const T& operator*()const& noexcept {
			return *_valuePtr;
		}
		T& operator*()& noexcept {
			return *_valuePtr;
		}
		const T&& operator*()const&& noexcept {
			return *_valuePtr;
		}
		T&& operator*() && noexcept{
			return *_valuePtr;
		}


		explicit operator bool() const noexcept {
			return _valuePtr != nullptr;
		}
		bool has_value() const noexcept {
			return _valuePtr != nullptr;
		}

		T& value()& {
			if (_valuePtr) {
				return *_valuePtr;
			}
			throw bad_optional_access(badOptionalAccessStr());
		}
		const T& value()const& {
			if (_valuePtr) {
				return *_valuePtr;
			}
			//throw bad_optional_access("Try access optional without value.");
			throw bad_optional_access(badOptionalAccessStr());
		}

		T&& value() && {
			if (_valuePtr) {
				return *_valuePtr;
			}
			//throw bad_optional_access("Try access optional without value.");
			throw bad_optional_access(badOptionalAccessStr());
		}
		const T&& value()const&& {
			if (_valuePtr) {
				return *_valuePtr;
			}
			//throw bad_optional_access("Try access optional without value.");
			throw bad_optional_access(badOptionalAccessStr());
		}

		// T must be CopyConstructible ...
		template<class U>
		T value_or(U&& value)const {
			if (_valuePtr) {
				return *_valuePtr;
			}
			else {
				return value;
			}
		}

	public: // Modifiers

		void swap(Optional& other) {
			if (has_value()) {
				if (other) {
					std::swap(*_valuePtr, *(other._valuePtr));
				}
				else {
					other._valuePtr = new (other._data) T(std::move(*_valuePtr));
					_valuePtr = nullptr;
				}
			}
			else {
				if (other) {
					_valuePtr = new (_data) T(std::move(*(other._valuePtr)));
					other._valuePtr = nullptr;
				}
			}
		}

		void reset()noexcept {
			if (_valuePtr) {
				_valuePtr->~T();
				_valuePtr = nullptr;
			}
		}

		template<class ... Args>
		void emplace(Args&& ... args) {
			reset();
			_valuePtr = new (_data) T(args...);
		}

	private:
		static const char* badOptionalAccessStr() {
			static auto str = std::string("Try access an instance of ") + typeid(Optional<T>).name() + " without value.";
			return str.c_str();
		}

		char _data[sizeof(T)];
		T* _valuePtr = nullptr;
	};

#pragma region Optional comparing operators
	// Compare two optional
	template<class T>
	bool operator==(const Optional<T>& lhs, const Optional<T>& rhs) {
		if (bool(lhs) != bool(rhs)) return false;
		if (!lhs) return true;
		return *lhs == *rhs;
	}

	template<class T>
	bool operator!=(const Optional<T>& lhs, const Optional<T>& rhs) {
		if (bool(lhs) != bool(rhs)) return true;
		if (!lhs) return false;
		return *lhs != *rhs;
	}

	template<class T>
	bool operator<(const Optional<T>& lhs, const Optional<T>& rhs) {
		if (!rhs) return false;
		if (!lhs) return true;
		return *lhs < *rhs;
	}

	template<class T>
	bool operator<=(const Optional<T>& lhs, const Optional<T>& rhs) {
		if (!lhs) return true;
		if (!rhs) return false;
		return *lhs <= *rhs;
	}

	template<class T>
	bool operator>(const Optional<T>& lhs, const Optional<T>& rhs) {
		if (!lhs) return false;
		if (!rhs) return true;
		return *lhs > *rhs;
	}

	template<class T>
	bool operator>=(const Optional<T>& lhs, const Optional<T>& rhs) {
		if (!rhs) return true;
		if (!lhs) return false;
		return *lhs >= *rhs;
	}

	// Compare optional with nullopt
	template<class T>
	bool operator==(const Optional<T>& opt, nullopt_t) noexcept {
		return !(bool(opt));
	}

	template<class T>
	bool operator==(nullopt_t, const Optional<T>& opt) noexcept {
		return !(bool(opt));
	}

	template<class T>
	bool operator!=(const Optional<T>& opt, nullopt_t) noexcept {
		return bool(opt);
	}

	template<class T>
	bool operator!=(nullopt_t, const Optional<T>& opt) noexcept {
		return bool(opt);
	}

	template<class T>
	bool operator<(const Optional<T>& opt, nullopt_t) noexcept {
		return false;
	}

	template<class T>
	bool operator<(nullopt_t, const Optional<T>& opt) noexcept {
		return bool(opt);
	}

	template<class T>
	bool operator<=(const Optional<T>& opt, nullopt_t) noexcept {
		return !(bool(opt));
	}

	template<class T>
	bool operator<=(nullopt_t, const Optional<T>& opt) noexcept {
		return true;
	}

	template<class T>
	bool operator>(const Optional<T>& opt, nullopt_t) noexcept {
		return bool(opt);
	}

	template<class T>
	bool operator>(nullopt_t, const Optional<T>& opt) noexcept {
		return false;
	}

	template<class T>
	bool operator>=(const Optional<T>& opt, nullopt_t) noexcept {
		return true;
	}

	template<class T>
	bool operator>=(nullopt_t, const Optional<T>& opt) noexcept {
		return !(bool(opt));
	}

	// Compare optional with value
	template<class T>
	bool operator==(const Optional<T>& opt, const T& value) noexcept {
		return bool(opt) ? *opt == value : false;
	}

	template<class T>
	bool operator==(const T& value, const Optional<T>& opt) noexcept {
		return bool(opt) ? value == *opt : false;
	}

	template<class T>
	bool operator!=(const Optional<T>& opt, const T& value) noexcept {
		return bool(opt) ? *opt != value : true;
	}

	template<class T>
	bool operator!=(const T& value, const Optional<T>& opt) noexcept {
		return bool(opt) ? value != *opt : true;
	}

	template<class T>
	bool operator<(const Optional<T>& opt, const T& value) noexcept {
		return bool(opt) ? *opt < value : true;
	}

	template<class T>
	bool operator<(const T& value, const Optional<T>& opt) noexcept {
		return bool(opt) ? value < *opt : false;
	}

	template<class T>
	bool operator<=(const Optional<T>& opt, const T& value) noexcept {
		return bool(opt) ? *opt <= value : true;
	}

	template<class T>
	bool operator<=(const T& value, const Optional<T>& opt) noexcept {
		return bool(opt) ? value <= *opt : false;
	}

	template<class T>
	bool operator>(const Optional<T>& opt, const T& value) noexcept {
		return bool(opt) ? *opt > value : false;
	}

	template<class T>
	bool operator>(const T& value, const Optional<T>& opt) noexcept {
		return bool(opt) ? value > *opt : true;
	}

	template<class T>
	bool operator>=(const Optional<T>& opt, const T& value) noexcept {
		return bool(opt) ? *opt >= value : false;
	}

	template<class T>
	bool operator>=(const T& value, const Optional<T>& opt) noexcept {
		return bool(opt) ? value >= *opt : true;
	}

#pragma endregion

	template<class T>
	Optional<std::decay_t<T>> make_optional(T&& value) {
		return Optional<std::decay_t<T>>(std::forward<T>(value));
	}

	template<class T, class ... Args>
	Optional<T> make_optional(Args&&... args) {
		return Optional<T>(in_place, std::forward<Args>(args)...);
	}

	template<class T>
	struct std::hash<Optional<T>> {
		using argument_type = Luxko::Optional<T>;
		using result_type = size_t;
		result_type operator()(const argument_type& opt)const {
			if (opt) return std::hash(*opt);
			else return std::hash(false);
		}
	};

	
	class bad_any_cast : public std::bad_cast {
	public:
		bad_any_cast() = default;
		virtual ~bad_any_cast() = default;
	};


	class Any {
	public:
		template<class VT>
		friend VT* any_cast(Any* pa);
		template<class VT>
		friend const VT* any_cast(const Any* pa);

		class any_content {
		public:
			any_content() = default;
			virtual ~any_content() = default;
			virtual const type_info& type()const noexcept {
				return typeid(void);
			}
			virtual std::unique_ptr<any_content> clone()const {
				return std::make_unique<any_content>();
			}

			virtual void* get()const {
				return nullptr;
			}
			//virtual std::unique_ptr<any_content> move() {
			//	return std::make_unique<any_content>();
			//}
		};

		template<class VT>
		class any_typed_content : public any_content {
		public:
			using value_type = VT;
			any_typed_content(const VT& v) : _data(v) {}
			any_typed_content(VT&& v) : _data(std::forward<VT>(v)) {}
			template<class ...Args>
			explicit any_typed_content(Args&&... args) : _data(std::forward<Args>(args)...) {}
			
			virtual ~any_typed_content() = default;

			virtual const type_info& type()const noexcept override {
				return typeid(value_type);
			}

			virtual std::unique_ptr<any_content> clone()const override {
				//return std::make_unique<any_typed_content<value_type>>(_data);
				return std::unique_ptr<any_content>(new any_typed_content<value_type>(_data));
			}

			virtual void* get()const {
				return reinterpret_cast<void*>(const_cast<VT*>(&_data));
			}

		public:
			VT _data;
		};

		Any()noexcept {}
		Any(const Any& other) {
			_content = other._content->clone();
		}
		Any(Any&& other)noexcept {
			_content = std::move(other._content);
		}

		template<class VT>
		Any(VT&& value) {
			_content = std::unique_ptr<any_content>(new any_typed_content<VT>(std::forward(value)));
		}

		template<class T, class ... Args>
		explicit Any(in_place_type_t<T>, Args&& ... args) {
			_content = std::unique_ptr<any_content>(new any_typed_content<T>(std::forward<Args>(args)...));
		}

		Any& operator=(const Any& rhs) {
			_content = rhs._content->clone();
			return *this;
		}
		Any& operator=(Any&& rhs)noexcept {
			_content = std::move(rhs._content);
			return *this;
		}
		template<typename VT>
		Any& operator=(VT&& rhs) {
			_content = std::unique_ptr<any_content>(new any_typed_content<VT>(std::forward(rhs)));
			return *this;
		}

		~Any() = default;

	public: // Modifiers
		template<class T, class ... Args>
		void emplace(Args&&... args) {
			_content = std::unique_ptr<any_content>(new any_typed_content<T>(std::forward<Args>(args)...));
		}

		void reset()noexcept {
			_content.reset();
		}

		void swap(Any& other)noexcept {
			_content.swap(other._content);
		}


		bool has_value() const noexcept {
			return _content.get() != nullptr;
		}

		const type_info& type()const noexcept {
			if(_content.get()) return _content->type();
			return typeid(void);
		}
	private:
		std::unique_ptr<any_content> _content;
	};

	
	template<class VT>
	VT any_cast(const Any& a) {
		auto ptr = any_cast<std::add_const_t<std::remove_reference_t<VT>>>(&a);
		if (ptr) return *ptr;
		throw bad_any_cast();
	}
	template<class VT>
	VT any_cast(Any& a) {
		auto ptr = any_cast<std::remove_reference_t<VT>>(&a);
		if (ptr) return *ptr;
		throw bad_any_cast();
	}

	template<class VT>
	VT any_cast(Any&& a) {
		auto ptr = any_cast<std::remove_reference_t<VT>>(&a);
		if (ptr) return *ptr;
		throw bad_any_cast();
	}


	template<class VT>
	const VT* any_cast(const Any* pa)/*noexcept*/ {
		if (pa && pa->has_value() && pa->type() == typeid(VT)) {
			return reinterpret_cast<VT*>(pa->_content->get());
		}
		return nullptr;
	}
	template<class VT>
	VT* any_cast(Any* pa)/*noexcept*/ {
		if (pa && pa->has_value() && pa->type() == typeid(VT)) {
			return reinterpret_cast<VT*>(pa->_content->get());
		}
		return nullptr;
	}

	template<class T, class... Args>
	Any make_any(Args&&...args) {
		return Any(in_place<T>, std::forward<Args>(args)...);
	}
}