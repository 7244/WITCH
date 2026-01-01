#if defined(__language_cpp)
  #ifndef remove_reference
    #define remove_reference remove_reference

    template<typename T>
    struct remove_reference {
      using type = T;
    };

    template<typename T>
    struct remove_reference<T&> {
      using type = T;
    };

    template<typename T>
    struct remove_reference<T&&> {
      using type = T;
    };
  #endif

  #ifndef remove_reference_t
    #define remove_reference_t remove_reference_t

    template<typename T>
    using remove_reference_t = typename remove_reference<T>::type;
  #endif

  #ifndef __is_type_same
    template <typename, typename>
    inline constexpr bool __is_type_same = false;
    template <typename _Ty>
    inline constexpr bool __is_type_same<_Ty, _Ty> = true;
    #define __is_type_same __is_type_same
  #endif

  #ifndef __dme
    #pragma pack(push, 1)

    template<typename T, const char* str, uintptr_t dt_size>
    struct __dme_t : T {
      const char *sn = str;
      uint32_t m_DSS = dt_size;
    };

    template<typename T>
    using return_type_of_t = decltype((*(T*)nullptr)());

    inline char __dme_empty_string[] = "";

    #define __dme(varname, ...) \
      struct varname##structed_dt{ \
        __VA_ARGS__ \
      }; \
      struct varname##_t : varname##structed_dt{ \
        constexpr operator uintptr_t() const { return __COUNTER__ - DME_INTERNAL__BEG - 1; } \
        static inline constexpr uintptr_t dss = (sizeof(#__VA_ARGS__) > 1) * sizeof(varname##structed_dt); \
      }; \
      inline static struct varname##_t varname; \
      static inline constexpr char varname##_str[] = #varname; \
      __dme_t<value_type, varname##_str, varname##_t::dss> varname##_ram

    template <typename main_t, uintptr_t index, typename T = __empty_struct>
    struct __dme_inherit_t{
      using value_type = T;
      using dme_type_t = __dme_t<value_type, __dme_empty_string, 0>;
      constexpr auto* NA(uintptr_t I) const { return &((dme_type_t *)this)[I]; }
      static constexpr uintptr_t GetMemberAmount() { return sizeof(main_t) / sizeof(dme_type_t); }
      static constexpr auto DME_INTERNAL__BEG = index;
    };

    #define __dme_inherit(main_t, ...) __dme_inherit_t<main_t, __COUNTER__, ##__VA_ARGS__>

    #pragma pack(pop)
  #endif
#endif
