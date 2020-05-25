// Components for manipulating non-owning sequences of characters -*- C++ -*-

// Copyright (C) 2013-2018 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file string_view
 *  This is a Standard C++ Library header.
 */

//
// N3762 basic_string_view library
//

#ifndef _GLIBCXX_STRING_VIEW
#define _GLIBCXX_STRING_VIEW 1

#pragma GCC system_header
#include <char_traits.hh>
#include <reverse_iterator.hh>

namespace std
{
  template<typename _CharT, typename _Traits = std::char_traits<_CharT>>
    class basic_string_view
    {
    public:

      // types
      using traits_type = _Traits;
      using value_type = _CharT;
      using pointer = const _CharT*;
      using const_pointer = const _CharT*;
      using reference = const _CharT&;
      using const_reference = const _CharT&;
      using const_iterator = const _CharT*;
      using iterator = const_iterator;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;
      using reverse_iterator = const_reverse_iterator;
      using size_type = size_t;
      using difference_type = ptrdiff_t;
      static constexpr size_type npos = size_type(-1);

      // [string.view.cons], construct/copy

      constexpr
      basic_string_view() noexcept
      : _M_len{0}, _M_str{nullptr}
      { }

      constexpr basic_string_view(const basic_string_view&) noexcept = default;

      constexpr basic_string_view(const _CharT* __str) noexcept
      : _M_len{__str == nullptr ? 0 : traits_type::length(__str)},
	_M_str{__str}
      { }

      constexpr
      basic_string_view(const _CharT* __str, size_type __len) noexcept
      : _M_len{__len}, _M_str{__str}
      { }

      constexpr basic_string_view&
      operator=(const basic_string_view&) noexcept = default;

      // [string.view.iterators], iterators

      constexpr const_iterator
      begin() const noexcept
      { return this->_M_str; }

      constexpr const_iterator
      end() const noexcept
      { return this->_M_str + this->_M_len; }

      constexpr const_iterator
      cbegin() const noexcept
      { return this->_M_str; }

      constexpr const_iterator
      cend() const noexcept
      { return this->_M_str + this->_M_len; }

      constexpr const_reverse_iterator
      rbegin() const noexcept
      { return const_reverse_iterator(this->end()); }

      constexpr const_reverse_iterator
      rend() const noexcept
      { return const_reverse_iterator(this->begin()); }

      constexpr const_reverse_iterator
      crbegin() const noexcept
      { return const_reverse_iterator(this->end()); }

      constexpr const_reverse_iterator
      crend() const noexcept
      { return const_reverse_iterator(this->begin()); }

      // [string.view.capacity], capacity

      constexpr size_type
      size() const noexcept
      { return this->_M_len; }

      constexpr size_type
      length() const noexcept
      { return _M_len; }

      constexpr size_type
      max_size() const noexcept
      {
	return (npos - sizeof(size_type) - sizeof(void*))
		/ sizeof(value_type) / 4;
      }

      [[nodiscard]] constexpr bool
      empty() const noexcept
      { return this->_M_len == 0; }

      // [string.view.access], element access

      constexpr const _CharT&
      operator[](size_type __pos) const noexcept
      {
	// TODO: Assert to restore in a way compatible with the constexpr.
	// __glibcxx_assert(__pos < this->_M_len);
	return *(this->_M_str + __pos);
      }

      constexpr const _CharT&
      at(size_type __pos) const
      {
	if (__pos >= _M_len)
	  __throw_out_of_range_fmt(__N("basic_string_view::at: __pos "
				       "(which is %zu) >= this->size() "
				       "(which is %zu)"), __pos, this->size());
	return *(this->_M_str + __pos);
      }

      constexpr const _CharT&
      front() const noexcept
      {
	// TODO: Assert to restore in a way compatible with the constexpr.
	// __glibcxx_assert(this->_M_len > 0);
	return *this->_M_str;
      }

      constexpr const _CharT&
      back() const noexcept
      {
	// TODO: Assert to restore in a way compatible with the constexpr.
	// __glibcxx_assert(this->_M_len > 0);
	return *(this->_M_str + this->_M_len - 1);
      }

      constexpr const _CharT*
      data() const noexcept
      { return this->_M_str; }

      // [string.view.modifiers], modifiers:

      constexpr void
      remove_prefix(size_type __n) noexcept
      {
	__glibcxx_assert(this->_M_len >= __n);
	this->_M_str += __n;
	this->_M_len -= __n;
      }

      constexpr void
      remove_suffix(size_type __n) noexcept
      { this->_M_len -= __n; }

      constexpr void
      swap(basic_string_view& __sv) noexcept
      {
	auto __tmp = *this;
	*this = __sv;
	__sv = __tmp;
      }


      // [string.view.ops], string operations:

      size_type
      copy(_CharT* __str, size_type __n, size_type __pos = 0) const
      {
	__glibcxx_requires_string_len(__str, __n);
	__pos = _M_check(__pos, "basic_string_view::copy");
	const size_type __rlen = std::min(__n, _M_len - __pos);
	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// 2777. basic_string_view::copy should use char_traits::copy
	traits_type::copy(__str, data() + __pos, __rlen);
	return __rlen;
      }

      constexpr basic_string_view
      substr(size_type __pos = 0, size_type __n = npos) const noexcept(false)
      {
	__pos = _M_check(__pos, "basic_string_view::substr");
	const size_type __rlen = std::min(__n, _M_len - __pos);
	return basic_string_view{_M_str + __pos, __rlen};
      }

      constexpr int
      compare(basic_string_view __str) const noexcept
      {
	const size_type __rlen = std::min(this->_M_len, __str._M_len);
	int __ret = traits_type::compare(this->_M_str, __str._M_str, __rlen);
	if (__ret == 0)
	  __ret = _S_compare(this->_M_len, __str._M_len);
	return __ret;
      }

      constexpr int
      compare(size_type __pos1, size_type __n1, basic_string_view __str) const
      { return this->substr(__pos1, __n1).compare(__str); }

      constexpr int
      compare(size_type __pos1, size_type __n1,
	      basic_string_view __str, size_type __pos2, size_type __n2) const
      {
	return this->substr(__pos1, __n1).compare(__str.substr(__pos2, __n2));
      }

      constexpr int
      compare(const _CharT* __str) const noexcept
      { return this->compare(basic_string_view{__str}); }

      constexpr int
      compare(size_type __pos1, size_type __n1, const _CharT* __str) const
      { return this->substr(__pos1, __n1).compare(basic_string_view{__str}); }

      constexpr int
      compare(size_type __pos1, size_type __n1,
	      const _CharT* __str, size_type __n2) const noexcept(false)
      {
	return this->substr(__pos1, __n1)
		   .compare(basic_string_view(__str, __n2));
      }

      constexpr size_type
      find(basic_string_view __str, size_type __pos = 0) const noexcept
      { return this->find(__str._M_str, __pos, __str._M_len); }

      constexpr size_type
      find(_CharT __c, size_type __pos = 0) const noexcept;

      constexpr size_type
      find(const _CharT* __str, size_type __pos, size_type __n) const noexcept;

      constexpr size_type
      find(const _CharT* __str, size_type __pos = 0) const noexcept
      { return this->find(__str, __pos, traits_type::length(__str)); }

      constexpr size_type
      rfind(basic_string_view __str, size_type __pos = npos) const noexcept
      { return this->rfind(__str._M_str, __pos, __str._M_len); }

      constexpr size_type
      rfind(_CharT __c, size_type __pos = npos) const noexcept;

      constexpr size_type
      rfind(const _CharT* __str, size_type __pos, size_type __n) const noexcept;

      constexpr size_type
      rfind(const _CharT* __str, size_type __pos = npos) const noexcept
      { return this->rfind(__str, __pos, traits_type::length(__str)); }

      constexpr size_type
      find_first_of(basic_string_view __str, size_type __pos = 0) const noexcept
      { return this->find_first_of(__str._M_str, __pos, __str._M_len); }

      constexpr size_type
      find_first_of(_CharT __c, size_type __pos = 0) const noexcept
      { return this->find(__c, __pos); }

      constexpr size_type
      find_first_of(const _CharT* __str, size_type __pos, size_type __n) const noexcept;

      constexpr size_type
      find_first_of(const _CharT* __str, size_type __pos = 0) const noexcept
      { return this->find_first_of(__str, __pos, traits_type::length(__str)); }

      constexpr size_type
      find_last_of(basic_string_view __str,
		   size_type __pos = npos) const noexcept
      { return this->find_last_of(__str._M_str, __pos, __str._M_len); }

      constexpr size_type
      find_last_of(_CharT __c, size_type __pos=npos) const noexcept
      { return this->rfind(__c, __pos); }

      constexpr size_type
      find_last_of(const _CharT* __str, size_type __pos,
		   size_type __n) const noexcept;

      constexpr size_type
      find_last_of(const _CharT* __str, size_type __pos = npos) const noexcept
      { return this->find_last_of(__str, __pos, traits_type::length(__str)); }

      constexpr size_type
      find_first_not_of(basic_string_view __str,
			size_type __pos = 0) const noexcept
      { return this->find_first_not_of(__str._M_str, __pos, __str._M_len); }

      constexpr size_type
      find_first_not_of(_CharT __c, size_type __pos = 0) const noexcept;

      constexpr size_type
      find_first_not_of(const _CharT* __str,
			size_type __pos, size_type __n) const noexcept;

      constexpr size_type
      find_first_not_of(const _CharT* __str, size_type __pos = 0) const noexcept
      {
	return this->find_first_not_of(__str, __pos,
				       traits_type::length(__str));
      }

      constexpr size_type
      find_last_not_of(basic_string_view __str,
		       size_type __pos = npos) const noexcept
      { return this->find_last_not_of(__str._M_str, __pos, __str._M_len); }

      constexpr size_type
      find_last_not_of(_CharT __c, size_type __pos = npos) const noexcept;

      constexpr size_type
      find_last_not_of(const _CharT* __str,
		       size_type __pos, size_type __n) const noexcept;

      constexpr size_type
      find_last_not_of(const _CharT* __str,
		       size_type __pos = npos) const noexcept
      {
	return this->find_last_not_of(__str, __pos,
				      traits_type::length(__str));
      }

      constexpr size_type
      _M_check(size_type __pos, const char* __s) const noexcept(false)
      {
	if (__pos > this->size())
	  __throw_out_of_range_fmt(__N("%s: __pos (which is %zu) > "
				       "this->size() (which is %zu)"),
				   __s, __pos, this->size());
	return __pos;
      }

      // NB: _M_limit doesn't check for a bad __pos value.
      constexpr size_type
      _M_limit(size_type __pos, size_type __off) const noexcept
      {
	const bool __testoff =  __off < this->size() - __pos;
	return __testoff ? __off : this->size() - __pos;
      }
      
    private:

      static constexpr int
      _S_compare(size_type __n1, size_type __n2) noexcept
      {
	const difference_type __diff = __n1 - __n2;
	if (__diff > std::numeric_limits<int>::max())
	  return std::numeric_limits<int>::max();
	if (__diff < std::numeric_limits<int>::min())
	  return std::numeric_limits<int>::min();
	return static_cast<int>(__diff);
      }

      size_t	    _M_len;
      const _CharT* _M_str;
    };

  // [string.view.comparison], non-member basic_string_view comparison function

  namespace __detail
  {
    // Identity transform to create a non-deduced context, so that only one
    // argument participates in template argument deduction and the other
    // argument gets implicitly converted to the deduced type. See n3766.html.
    template<typename _Tp>
      using __idt = common_type_t<_Tp>;
  }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator==(basic_string_view<_CharT, _Traits> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.size() == __y.size() && __x.compare(__y) == 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator==(basic_string_view<_CharT, _Traits> __x,
               __detail::__idt<basic_string_view<_CharT, _Traits>> __y) noexcept
    { return __x.size() == __y.size() && __x.compare(__y) == 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator==(__detail::__idt<basic_string_view<_CharT, _Traits>> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.size() == __y.size() && __x.compare(__y) == 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator!=(basic_string_view<_CharT, _Traits> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return !(__x == __y); }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator!=(basic_string_view<_CharT, _Traits> __x,
               __detail::__idt<basic_string_view<_CharT, _Traits>> __y) noexcept
    { return !(__x == __y); }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator!=(__detail::__idt<basic_string_view<_CharT, _Traits>> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return !(__x == __y); }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator< (basic_string_view<_CharT, _Traits> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.compare(__y) < 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator< (basic_string_view<_CharT, _Traits> __x,
               __detail::__idt<basic_string_view<_CharT, _Traits>> __y) noexcept
    { return __x.compare(__y) < 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator< (__detail::__idt<basic_string_view<_CharT, _Traits>> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.compare(__y) < 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator> (basic_string_view<_CharT, _Traits> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.compare(__y) > 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator> (basic_string_view<_CharT, _Traits> __x,
               __detail::__idt<basic_string_view<_CharT, _Traits>> __y) noexcept
    { return __x.compare(__y) > 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator> (__detail::__idt<basic_string_view<_CharT, _Traits>> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.compare(__y) > 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator<=(basic_string_view<_CharT, _Traits> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.compare(__y) <= 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator<=(basic_string_view<_CharT, _Traits> __x,
               __detail::__idt<basic_string_view<_CharT, _Traits>> __y) noexcept
    { return __x.compare(__y) <= 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator<=(__detail::__idt<basic_string_view<_CharT, _Traits>> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.compare(__y) <= 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator>=(basic_string_view<_CharT, _Traits> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.compare(__y) >= 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator>=(basic_string_view<_CharT, _Traits> __x,
               __detail::__idt<basic_string_view<_CharT, _Traits>> __y) noexcept
    { return __x.compare(__y) >= 0; }

  template<typename _CharT, typename _Traits>
    constexpr bool
    operator>=(__detail::__idt<basic_string_view<_CharT, _Traits>> __x,
               basic_string_view<_CharT, _Traits> __y) noexcept
    { return __x.compare(__y) >= 0; }

  // [string.view.io], Inserters and extractors
  template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>&
    operator<<(basic_ostream<_CharT, _Traits>& __os,
	       basic_string_view<_CharT,_Traits> __str)
    { return __ostream_insert(__os, __str.data(), __str.size()); }


  // basic_string_view typedef names

  using string_view = basic_string_view<char>;
#ifdef _GLIBCXX_USE_WCHAR_T
  using wstring_view = basic_string_view<wchar_t>;
#endif
#ifdef _GLIBCXX_USE_C99_STDINT_TR1
  using u16string_view = basic_string_view<char16_t>;
  using u32string_view = basic_string_view<char32_t>;
#endif

  // [string.view.hash], hash support:

  template<typename _Tp>
    struct hash;

  template<>
    struct hash<string_view>
    : public __hash_base<size_t, string_view>
    {
      size_t
      operator()(const string_view& __str) const noexcept
      { return std::_Hash_impl::hash(__str.data(), __str.length()); }
    };

  template<>
    struct __is_fast_hash<hash<string_view>> : std::false_type
    { };

#ifdef _GLIBCXX_USE_WCHAR_T
  template<>
    struct hash<wstring_view>
    : public __hash_base<size_t, wstring>
    {
      size_t
      operator()(const wstring_view& __s) const noexcept
      { return std::_Hash_impl::hash(__s.data(),
                                     __s.length() * sizeof(wchar_t)); }
    };

  template<>
    struct __is_fast_hash<hash<wstring_view>> : std::false_type
    { };
#endif

#ifdef _GLIBCXX_USE_C99_STDINT_TR1
  template<>
    struct hash<u16string_view>
    : public __hash_base<size_t, u16string_view>
    {
      size_t
      operator()(const u16string_view& __s) const noexcept
      { return std::_Hash_impl::hash(__s.data(),
                                     __s.length() * sizeof(char16_t)); }
    };

  template<>
    struct __is_fast_hash<hash<u16string_view>> : std::false_type
    { };

  template<>
    struct hash<u32string_view>
    : public __hash_base<size_t, u32string_view>
    {
      size_t
      operator()(const u32string_view& __s) const noexcept
      { return std::_Hash_impl::hash(__s.data(),
                                     __s.length() * sizeof(char32_t)); }
    };

  template<>
    struct __is_fast_hash<hash<u32string_view>> : std::false_type
    { };
#endif

  inline namespace literals
  {
  inline namespace string_view_literals
  {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
    inline constexpr basic_string_view<char>
    operator""sv(const char* __str, size_t __len) noexcept
    { return basic_string_view<char>{__str, __len}; }

#ifdef _GLIBCXX_USE_WCHAR_T
    inline constexpr basic_string_view<wchar_t>
    operator""sv(const wchar_t* __str, size_t __len) noexcept
    { return basic_string_view<wchar_t>{__str, __len}; }
#endif

#ifdef _GLIBCXX_USE_C99_STDINT_TR1
    inline constexpr basic_string_view<char16_t>
    operator""sv(const char16_t* __str, size_t __len) noexcept
    { return basic_string_view<char16_t>{__str, __len}; }

    inline constexpr basic_string_view<char32_t>
    operator""sv(const char32_t* __str, size_t __len) noexcept
    { return basic_string_view<char32_t>{__str, __len}; }
#endif
  } // namespace string_literals
  } // namespace literals

} // namespace std


#endif // _GLIBCXX_EXPERIMENTAL_STRING_VIEW
