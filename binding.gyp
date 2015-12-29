{
  "targets": [
    {
      "target_name": "genx",
      "sources": [
        "src/charProps.cc",
        "src/genx.cc",
        "src/namespace.cc",
        "src/element.cc",
        "src/attribute.cc",
        "src/writer.cc",
        "src/node-genx.cc"
      ],
      "include_dirs" : [ "<!(node -e \"require('nan')\")" ]
    }
  ]
}

