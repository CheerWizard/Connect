package com.cws.connect

data class AudioConfig(
    val supportRecording: Boolean,
    val sampleRate: Int,
    val sampleBufferSize: Int,
    val delayInMillis: Long,
    val decay: Float
)