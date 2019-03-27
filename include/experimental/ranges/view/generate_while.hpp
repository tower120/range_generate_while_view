#pragma once

#include <range/v3/all.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/view_facade.hpp>


namespace experimental::ranges::view{

    template<class Fn>
    class generate_while_view
        : public ::ranges::view_facade<generate_while_view<Fn>, ::ranges::unknown>
    {
        friend ::ranges::range_access;
        Fn fn_;

        // val_t = optional<T>
        using optional_result_t = ::ranges::invoke_result_t<Fn&, std::true_type>;
        using result_t = typename optional_result_t::value_type;

        ::ranges::detail::non_propagating_cache<result_t> val_;

        struct cursor
        {
        private:
            generate_while_view* view_;

            void optimized_emplace(optional_result_t&& res){
                view_->val_.reset();
                if (res.has_value()){
                    view_->val_.emplace(std::move(res.value()));
                }
            }

            template<bool rewind>
            void update_cache(){
                //view_->val_ = std::move(std::invoke(view_->fn_, std::bool_constant<rewind>{}));
                optimized_emplace(std::invoke(view_->fn_, std::bool_constant<rewind>{}));
            }
        public:
            cursor() = default;
            explicit cursor(generate_while_view &view)
              : view_(&view)
            {
                update_cache<true>();
            }
            bool equal(::ranges::default_sentinel) const
            {
                return !view_->val_;
            }
            result_t&& read() const
            {
                return static_cast<result_t &&>(
                        static_cast<result_t &>(*view_->val_));
            }
            void next()
            {
                update_cache<false>();
            }
        };
        cursor begin_cursor()
        {
            return cursor{*this};
        }

    public:
        generate_while_view() = default;
        explicit generate_while_view(Fn fn)
            : fn_(std::move(fn))
        {}
    };

    struct generate_while_fn
    {
        template<typename Fn>
        decltype(auto) operator()(Fn fn) const
        {
            return generate_while_view<Fn>( std::move(fn) );
        }
    };

	inline constexpr const generate_while_fn generate_while;
}