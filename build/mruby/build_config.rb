
MRuby::CrossBuild.new('rtl8196') do |conf|
  toolchain :gcc
  conf.cc.command = 'mips-cc'
  conf.archiver.command = 'mips-ar'

  cc.defines << %w(MRB_NO_STDIO)
  cc.defines << %w(MRB_NO_FLOAT)
  cc.defines << %w(YABM_REALTEK)
  conf.cc.flags << "-march=4181"
  conf.cc.flags << "-g -fno-pic -mno-abicalls"
  conf.cc.flags << "-pipe -mlong-calls"
#  conf.cc.flags << "-mips16"
  conf.cc.include_paths = ["#{root}/include", "../build/work/newlib-3.0.0.20180831/newlib/libc/include"]

  conf.gem :github => 'yamori813/mruby-yabm'
  conf.gem :github => 'yamori813/mruby-simplehttp'
# use in mruby-simplehttp'
  conf.gem :core => "mruby-string-ext"
end

MRuby::CrossBuild.new('rtl8198') do |conf|
  toolchain :gcc
  conf.cc.command = 'mips-cc'
  conf.archiver.command = 'mips-ar'

  cc.defines << %w(MRB_METHOD_T_STRUCT)
  cc.defines << %w(MRB_DISABLE_STDIO)
  cc.defines << %w(MRB_WITHOUT_FLOAT)
  cc.defines << %w(YABM_REALTEK)
  conf.cc.flags << "-march=5281"
  conf.cc.flags << "-g -fno-pic -mno-abicalls"
  conf.cc.flags << "-pipe -mlong-calls"
#  conf.cc.flags << "-mips16"
  conf.cc.include_paths = ["#{root}/include", "../build/work/newlib-3.0.0.20180831/newlib/libc/include"]

  conf.gem :github => 'yamori813/mruby-yabm'
  conf.gem :github => 'yamori813/mruby-simplehttp'
#  conf.gem :github => 'yamori813/mruby-simplehttp'
# use in mruby-simplehttp'
  conf.gem :core => "mruby-string-ext"
end
