settings = NewSettings()

settings.cc.includes:Add("include")
src = Collect("src/*.c")
bam = Link(settings, "memsearch", Compile(settings, src))
