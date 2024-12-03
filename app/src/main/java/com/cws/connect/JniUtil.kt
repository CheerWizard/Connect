package com.cws.connect

import java.lang.reflect.Method

object JniUtil {

    private val PRIMITIVE_SIGNATURES: MutableMap<Any?, String> = HashMap()

    init {
        PRIMITIVE_SIGNATURES[Boolean::class.javaPrimitiveType] = "Z"
        PRIMITIVE_SIGNATURES[Byte::class.javaPrimitiveType] = "B"
        PRIMITIVE_SIGNATURES[Char::class.javaPrimitiveType] = "C"
        PRIMITIVE_SIGNATURES[Double::class.javaPrimitiveType] = "D"
        PRIMITIVE_SIGNATURES[Float::class.javaPrimitiveType] = "F"
        PRIMITIVE_SIGNATURES[Int::class.javaPrimitiveType] = "I"
        PRIMITIVE_SIGNATURES[Long::class.javaPrimitiveType] = "J"
        PRIMITIVE_SIGNATURES[Short::class.javaPrimitiveType] = "S"
        PRIMITIVE_SIGNATURES[Void.TYPE] = "V"
    }

    fun getJNIMethodSignature(m: Method): String {
        val sb = StringBuilder("(")
        for (p in m.parameterTypes) {
            sb.append(getJNIClassSignature(p))
        }
        sb.append(')').append(getJNIClassSignature(m.returnType))
        return sb.toString()
    }

    fun getJNIClassSignature(c: Class<*>): String? {
        if (c.isArray) {
            val ct = c.componentType
            return '['.toString() + getJNIClassSignature(ct)
        } else if (c.isPrimitive) {
            return PRIMITIVE_SIGNATURES[c]
        } else {
            return 'L'.toString() + c.name.replace('.', '/') + ';'
        }
    }
}