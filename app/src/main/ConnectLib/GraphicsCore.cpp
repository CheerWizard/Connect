//
// Created by cheerwizard on 02.12.24.
//

#include "GraphicsCore.hpp"

void AttributeLayout::bind() {

}

void AttributeLayout::unbind() {

}

void GeometryBuffer::init() {

}

void GeometryBuffer::free() {

}

GeometryDrawData GeometryBuffer::addGeometry(const GeometryData &geometryData) {
    return GeometryDrawData();
}

void GeometryBuffer::removeGeometry(const GeometryDrawData &geometryDrawData) {

}

void GeometryBuffer::flush() {

}

Shader::Shader() {

}

Shader::~Shader() {

}

void Shader::run() {
    attributeLayout.bind();

    attributeLayout.unbind();
}