#include "ButtonState.h"

ButtonState::ButtonState(const QIcon &icon, const QString &text) : icon(icon), text(text) {
}

ButtonState::ButtonState(const QIcon &icon) : icon(icon) {
}

ButtonState::ButtonState(const QString &text) : text(text) {
}

ButtonState::ButtonState() {
}

const QIcon &ButtonState::getIcon() const {
    return this->icon;
}

const QString &ButtonState::getText() const {
    return this->text;
}
