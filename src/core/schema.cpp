#include "schema.h"

Schema::Schema(const QString &name) : m_name(name) {
}

void Schema::addField(const Field &field) {
    m_fields.append(field);
}

QString Schema::getName() const {
    return m_name;
}

const QList<Field>& Schema::getFields() const {
    return m_fields;
}