#ifndef OUR_UTILS_IMGUI_H
#define OUR_UTILS_IMGUI_H

#include <functional>
#include <iterator>
#include <optional>
#include <map>

#include <glad/gl.h>
#include <imgui.h>
#include <glm/common.hpp>

namespace our {

    namespace gl_enum_options {
        typedef std::map<GLenum, const char*> const EnumMap;

        inline EnumMap primitives = {
                {GL_POINTS, "GL_POINTS"},
                {GL_LINES, "GL_LINES"},
                {GL_LINE_STRIP, "GL_LINE_STRIP"},
                {GL_LINE_LOOP, "GL_LINE_LOOP"},
                {GL_TRIANGLES, "GL_TRIANGLES"},
                {GL_TRIANGLE_STRIP, "GL_TRIANGLE_STRIP"},
                {GL_TRIANGLE_FAN, "GL_TRIANGLE_FAN"},
        };

        inline EnumMap polygon_modes = {
                {GL_POINT, "GL_POINT"},
                {GL_LINE, "GL_LINE"},
                {GL_FILL, "GL_FILL"}
        };

    }

    template<typename T>
    static void OptionMapCombo(const char* label, T& selected, const std::map<T, const char*>& options){
        if(ImGui::BeginCombo(label, options.at(selected))){
            for(auto const& [key, value] : options){
                bool is_selected = selected == key;
                if(ImGui::Selectable(value, is_selected))
                    selected = key;
                if(is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }


    static bool ColorEdit4U8(const char* label, uint8_t* color, ImGuiColorEditFlags flags = 0){
        float color_f32[4] = { color[0]/255.0f, color[1]/255.0f, color[2]/255.0f, color[3]/255.0f };
        bool result = ImGui::ColorEdit4(label, color_f32, flags);
        color[0] = 255 * color_f32[0];
        color[1] = 255 * color_f32[1];
        color[2] = 255 * color_f32[2];
        color[3] = 255 * color_f32[3];
        return result;
    }

    template<typename It>
    static void ReorderableList(It begin, It end,
                                std::function<void(size_t,typename std::iterator_traits<It>::value_type&)> item_gui,
                                std::function<void(size_t)> item_add,
                                std::function<void(size_t)> item_delete,
                                size_t starting_index = 0){
        size_t index = starting_index;
        size_t distance = std::distance(begin, end);
        size_t last = starting_index + distance - 1;
        std::optional<size_t> item_to_delete, item_to_add;
        std::optional<std::tuple<It, It>> items_to_swap;
        for(It it = begin, prev; it != end; prev = it, it++, ++index){
            ImGui::PushID(index);

            if(ImGui::Button("+"))
                item_to_add = index;
            ImGui::Separator();

            item_gui(index, *it);

            if(ImGui::Button("-"))
                item_to_delete = index;
            ImGui::SameLine();
            if(ImGui::Button("^") && index > starting_index)
                items_to_swap = { prev, it };
            ImGui::SameLine();
            if(ImGui::Button("v") && index < last)
                items_to_swap = { it, it + 1 };
            ImGui::Separator();
            ImGui::PopID();
        }
        if(ImGui::Button("+"))
            item_to_add = index;

        if(item_to_add.has_value())
            item_add(item_to_add.value());
        else if(item_to_delete.has_value())
            item_delete(item_to_delete.value());
        else if(items_to_swap.has_value()){
            auto [it1, it2] = items_to_swap.value();
            auto temp = *it1;
            *it1 = *it2;
            *it2 = temp;
        }

    }

}

#endif //OUR_UTILS_IMGUI_H
