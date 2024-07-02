Rebol [
    title: "Rebol/AOMedia extension CI test"
]

print ["Running test on Rebol build:" mold to-block system/build]
system/options/quiet: false
system/options/log/rebol: 4

;; make sure that we load a fresh extension
try [system/modules/aomedia: none]
;; use current directory as a modules location
system/options/modules: what-dir

;; import the extension
aom: import 'aomedia

print as-yellow "Content of the module..."
? aom


print as-yellow "Encode some frames..."
enc: aom/make-encoder 640x480
aom/encode-frame :enc make image! [640x480 255.0.0]
aom/encode-frame :enc make image! [640x480 0.255.0]
aom/encode-frame :enc make image! [640x480 0.0.255]
write %video.obu aom/encode-frame :enc none