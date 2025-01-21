//
// @project     sdk-explorer
//
// @author      ElCapor
//
// @license     CC BY-NC-SA 4.0
//
#include <imgui.h>
#include <vector>
namespace ui
{

    /**
     * @brief An item that can be used in a FilterBox
     * 
     */
    template <typename InternalType = int32_t>
    struct FilterItem
    {
        /**
         * @brief The name that will be shown in the item box
         * 
         * @return const char* 
         */
        virtual const char* GetName() = 0;
        
        /**
         * @brief Unique id for this item
         * 
         * @return ImGuiID 
         */
        virtual ImGuiID UniqueID() = 0;
    };

    /**
     * @brief A DualListFilterBox widget. Allows to create dual list box easily
     * @note Features 3 Behaviours: Move All , Move Single, MoveSelected, 
     * @tparam InternalType 
     */
    template <typename InternalType = FilterItem<int32_t>>
    struct DualListFilterBox {
        /*
        This holds the items for each selection box.
        0 is avaliable
        1 is selected
        I call an std::vector<InternalType> a "Basket"
        Defaulting to int means you're storing numbers,
        which can be potentially indices in a table for example
        */
        std::vector<InternalType> Items[2];
        /*
        Optional helper to store multi-selection state + apply multi-selection requests.
        */
       ImGuiSelectionBasicStorage Selections[2];

        /**
         * @brief Move all items from one selection to another
         * 
         * @param src Source Basket
         * @param dst Destination Basket
         */
       void MoveAll(int src, int dst)
       {
            // Make sure the source and destination are valid
            IM_ASSERT((src >= 0 && src < 2) && (dst >= 0 && dst < 2) && (dst != src));
            for (InternalType& item : Items[src])
                Items[dst].push_back(item); // Move all items from one selection to another
            Items[src].clear(); // Clear the source basket
            Selections[src].Swap(Selections[dst]); // Swap the selections
            Selections[src].Clear(); // Clear the source selection
       }

       /**
        * @brief Move all selected items from one selection to another
        * 
        * @param src Source Basket
        * @param dst Destination Basket
        */
        void MoveSelected(int src, int dst)
        {
            // Make sure the source and destination are valid
            IM_ASSERT((src >= 0 && src < 2) && (dst >= 0 && dst < 2) && (dst != src));
            
            for (int src_index = Items[src].size() - 1; src_index >= 0; src_index--)
            {
                InternalType item = Items[src][src_index];
                if (!Selections[src].Contains(item))
                    continue;
                
                Items[dst].push_back(item);
                Items[src].erase(src_index);
            }
            Selections[src].Clear();
        }

        /**
         * @brief Draw the widget
         * 
         */
        void Draw()
        {
            if (ImGui::BeginTable("DualListFilterBox", 3, ImGuiTableFlags_None))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch); // Left side
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);	 // Buttons
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch); // Right side
                ImGui::TableNextRow();

                for (int side=0; side<2; side++)
                {
                    std::vector<InternalType>& basket = Items[side];
                    ImGuiSelectionBasicStorage& selection = Selections[side];
                    ImGui::TableSetColumnIndex((side == 0) ? 0 : 2); // we are left or right ig
                    ImGui::Text("%s", side == 0 ? "Available" : "Selected");

                    const float items_height = ImGui::GetTextLineHeightWithSpacing() * basket.size();
                    ImGui::SetNextWindowContentSize(ImVec2(0.0f, items_height));

                    // this is same as ImGui::BeginChild
                    bool current_child;
                    float left_side_height = 0.0f; // because right side acts based on left side
                    if (side == 0)
                    {
					    ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetFrameHeightWithSpacing() * 4), ImVec2(FLT_MAX, FLT_MAX));
                        current_child = ImGui::BeginChild("0", ImVec2(-FLT_MIN, ImGui::GetFontSize() * 20), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_ResizeY);
                        left_side_height = ImGui::GetItemRectSize().y;

                    } else{
                        current_child = ImGui::BeginChild("1", ImVec2(-FLT_MIN, left_side_height), ImGuiChildFlags_FrameStyle);
                    }
                    if (current_child)
                    {
                        ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_None;
                        ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(flags, selection.Size, basket.size());

                        for (int item_idx = 0; item_idx < basket.size(); item_idx++)
                        {
                            InternalType item = basket[item_idx];
                            bool is_selected = selection.Contains(item);
                            ImGui::SetNextItemSelectionUserData(item_idx);
                            ImGui::Selectable(item.GetName(), is_selected, ImGuiSelectableFlags_AllowDoubleClick);
                            if (ImGui::IsItemFocused())
                            {
                                if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsMouseDoubleClicked(0))
                                    printf("pls move");
                            }
                        }

                        ms_io = ImGui::EndMultiSelect();
                    }
                }
            }
        }
    };
}