#pragma once

namespace tremolo {
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
  enum class Colors : size_t { orange, paleBlue };

  static juce::Colour getColor(Colors colorName);

  CustomLookAndFeel();

  [[nodiscard]] static juce::FontOptions getSideLabelsFont();

  juce::BorderSize<int> getLabelBorderSize(juce::Label&) override;

  [[nodiscard]] static juce::FontOptions getRateLabelFont();

  void drawRotarySlider(juce::Graphics&,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPosProportional,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        juce::Slider&) override;

  void drawComboBox(juce::Graphics&,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox&) override;

  juce::Font getComboBoxFont(juce::ComboBox&) override;

  /** @brief Positions the Label within the ComboBox and sets its font */
  void positionComboBoxText(juce::ComboBox&,
                            juce::Label& labelToPosition) override;

  juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(
      juce::ComboBox& box,
      juce::Label& label) override;

  juce::Font getPopupMenuFont() override;

  juce::Path getTickShape(float) override;

  void drawToggleButton(juce::Graphics&,
                        juce::ToggleButton&,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;

private:
  [[nodiscard]] static juce::FontOptions interMedium();
  [[nodiscard]] static juce::FontOptions interBold();
};
}  // namespace tremolo
