settings = NewSettings()

settings.cc.includes:Add("include")
src = Collect("src/*.c")

settings.cc.Output = function(settings, input)
    return 'build/' .. settings.config_name .. '/' .. PathBase(input) .. settings.config_ext
end

settings.config_name = 'dbg'

bam_dbg = Link(settings, "memsearch" .. '_' .. settings.config_name, Compile(settings, src))

settings.optimize = 1
settings.config_name = 'opt'

bam_opt = Link(settings, "memsearch" .. '_' .. settings.config_name, Compile(settings, src))

