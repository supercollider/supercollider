#!/usr/bin/ruby

#scvim_make_help.rb
#Written by Alex Norman for scvim with help from John Yates and Andrzej Kopec
#This script uses the apps (unhtml or links) and urtf, it does some wrapping around unhtml
#because that doesn't remove the stuff from html <head>..</head>
#it also wraps unrtf because it has some bugs
#
#it removes the unhtml formatting and puts the results in the specified directory

#you can edit these if you know what you're doing
#use links for default unhtml [otherwise use "unhtml"]
#the trade off is that links might provide nicer output sometimes but it breaks
#up paragraphs with newlines so if you have a small help window [width wise] the
#text will look lame
#
#
#Copyright 2007 Alex Norman
#
#This file is part of SCVIM.
#
#SCVIM is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#SCVIM is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with SCVIM.  If not, see <http://www.gnu.org/licenses/>.

$uselinks = false
scvim_cache_dir = ENV["SCVIM_CACHE_DIR"]
if !scvim_cache_dir or scvim_cache_dir==""
  scvim_cache_dir = File.join(ENV["HOME"],".scvim")
else
  scvim_cache_dir.sub!(/\/$/,"")
end

#if the cache dir doesn't exist, create it
if not File.exists?(scvim_cache_dir)
  Dir.mkdir(scvim_cache_dir)
end

help_dest = File.join(scvim_cache_dir, "/doc/")
help_tags_file = File.join(help_dest, "TAGS_HELP")
help_completion_file = File.join(help_dest, "sc_help_completion")
$unhtml = "unhtml"
$unrtf = "unrtf"

help_source_list = Array.new

require 'open3'
require 'tempfile'
require 'optparse'

#create a new hash for the help
helphash = Hash.new;

def deleteRecursively(items_to_del)
  items_to_del.each {|x| 
    if File.stat(x).directory?
      new_items_to_del = Dir.entries(x)
      new_items_to_del.delete("."); new_items_to_del.delete(".."); new_items_to_del.delete(".svn")
      new_items_to_del.collect! {|i| "#{x}/" + i }
      deleteRecursively(new_items_to_del)
      Dir.unlink(x)
    else
      File.delete(x)
    end
  }
end

def deleteEmptyDirs(basedir)
  basedir.each {|x| 
    if File.stat(x).directory?
      items_to_del = Dir.entries(x)
      items_to_del.delete("."); items_to_del.delete(".."); items_to_del.delete(".svn")
      items_to_del.collect! {|i| "#{x}/" + i }
      #first check any subdirs if there are any
      if items_to_del != []
        deleteEmptyDirs(items_to_del)
        items_to_del = Dir.entries(x)
        items_to_del.delete("."); items_to_del.delete(".."); items_to_del.delete(".svn")
        if items_to_del == []
          Dir.unlink(x)
        end
      else
        Dir.unlink(x)
      end
    end
  }
end


def runhtml(file)
  File.open(file, "r"){|f|
    lines = f.readlines
    bodyStart = 0
    #ditch the header because unhtml doesn't deal with it well
    (0..lines.size - 1).each do |i|
      bodyStart = i
      if lines[i] =~ /<body/
        break
      end
    end
    #unhtml everything from the body on
    stdin, stdout, stderr = Open3.popen3($unhtml)
    stdin.puts lines[bodyStart..lines.size - 1]
    stdin.close
    #grab the lines from unhtml
    #res = stdout.readlines
    res = stdout.readlines.collect { |i| 
      i.gsub!(/[\x80-\xFF]/, '')
      i.gsub!("&ndash;",'-')
      i
    }
    tmp = Array.new
    res.each { |i|
      #remove html comments
      if i !~ /^\s*<!--.*-->\s*$/
        tmp << i
      end
    }
    #close everything
    stdout.close; stderr.close
    return tmp
  }
  return []
end

def myunrtf(infile,outfile)
  tf = Tempfile.new("scvim_unrtfing")
  tf2 = Tempfile.new("scvim_unrtfing")
  File.open(infile,"r") { |f|
    #work around unrtf's lame exclusion of brackets
    while !f.eof
      line = f.gets
      line.gsub!(/\\\{/,"OPENBRACKET")
      line.gsub!(/\\\}/,"CLOSEBRACKET")
      line.gsub!(/\\\\/,"SLASH")
      tf.puts line
    end
  }
  #close the temp file
  tf.close(false)
  tf2.close(false)
  system("#{$unrtf} --text #{tf.path} > #{tf2.path} 2> /dev/null")
  tf2.open
  File.open(outfile,"w") { |f|
    #work around unrtf's lame exclusion of brackets
    count = 0
    while !tf2.eof
      line = tf2.gets
      #if the line isn't a comment
      if line !~ /^\s*###/
        #remove the blank line and line with ----- at they are there
        if(!(count < 2 && (line !~ /^\s*$/ || line !~ /^-+/)))
          line.gsub!(/OPENBRACKET/,"{")
          line.gsub!(/CLOSEBRACKET/,"}")
          line.gsub!(/SLASH/,"\\")
          if line.gsub!(/###.*$/,"")
            line.chomp!
          end
          f.print line
        end
        count += 1
      end
    end
  }
end

def myunhtml(infile,outfile)
  if $uselinks 
    lines = IO.popen("links -dump -width 128 'file://#{infile}'").readlines
    #remove annoying spaces at beg of each line, first see if they are there
    hasAnnoyingSpaces = true
    lines.each do |line|
      if line !~ /^\s{3,}/ and line !~ /^\s*$/
        hasAnnoyingSpaces = false
        break
      end
    end
    if hasAnnoyingSpaces
      oldlines = lines
      lines = Array.new
      oldlines.each do |line|
        lines << line.sub(/^\s{3,3}/, "")
      end
    end
    File.open(outfile, "w") { |f|
      f.puts lines
    }
  else
    File.open(outfile, "w") { |f|
      f.puts runhtml(infile)
    }
  end
end

def processFiles(files_to_process, basedir, helphash, destdir)
  files_to_process.each{|file|
    dir = "#{basedir}/" + file
    if File.stat(dir).directory?
      if file !~ /CVS|\.svn/
        if file =~ /\.rtfd/
          #remove the .rtf
          helpitem = file.sub(/\.rtfd/,"")
          #if it is suffixed with ".help" remove that
          helpitem.sub!(/\.help/,"")
          #remove whitespace
          helpitem.gsub!(/\s+/, "_")
          output_helpfile = File.join(destdir, "#{helpitem}.scd")
          #see if there is a file here, 
          if !File.exists?(output_helpfile) && helphash.key?(helpitem)
            puts "** #{helpitem} has multiple files associated with it, overwriting the old one"
          end
          if File.exists?("#{basedir}/#{file}/TXT.rtf")
            myunrtf("#{basedir}/#{file}/TXT.rtf",output_helpfile)
            helphash[helpitem] = output_helpfile
          end
        else
          dirfiles = Dir.entries(dir)
          dirfiles.delete("."); dirfiles.delete("..")
          newdestdir = File.join(destdir, file)
          newdestdir.gsub!(/\s+/, "_")
          #we must make this directory if it doesn't already exist
          if !File.exists?(newdestdir)
            Dir.mkdir(newdestdir)
          elsif !File.stat(newdestdir).directory?
            puts "There is a filename that exists that we want to be a directory: #{newdestdir}, exiting\n"
            exit -1
          end
          processFiles(dirfiles, dir, helphash, newdestdir)
        end
      end
    else
      if file =~ /\.rtf/
        #remove the .rtf
        helpitem = file.sub(/\.rtf/,"")
        #if it is suffixed with ".help" remove that
        helpitem.sub!(/\.help/,"")
        #remove whitespace
        helpitem.gsub!(/\s+/, "_")
        output_helpfile = "#{destdir}/#{helpitem}.scd"
        #see if there is a file here that is already up to date
        if !File.exists?(output_helpfile) && helphash.key?(helpitem)
          puts "** #{helpitem} has multiple files associated with it, overwriting the old one"
        end
        myunrtf(File.join(basedir,file), output_helpfile)
        helphash[helpitem] = output_helpfile
      elsif file =~ /\.htm[l]?/
        #remove the .html
        helpitem = file.sub(/\.htm[l]?/,"")
        #if it is suffixed with ".help" remove that
        helpitem.sub!(/\.help/,"")
        #remove whitespace
        helpitem.gsub!(/\s+/, "_")
        output_helpfile = File.join(destdir, "#{helpitem}.scd")
        #see if there is a file here that is already up to date
        if !File.exists?(output_helpfile) && helphash.key?(helpitem)
          puts "** #{helpitem} has multiple files associated with it, overwriting the old one"
        end
        myunhtml(File.join(basedir,file), output_helpfile)
        helphash[helpitem] = output_helpfile
      elsif file =~ /\.scd/
        output_helpfile = "#{destdir}/#{file}"
        helpitem = file.sub(/\.scd/,"")
        #remove whitespace
        helpitem.gsub!(/\s+/, "_")
        if !File.exists?(output_helpfile) && helphash.key?(helpitem)
          puts "** #{helpitem} has multiple files associated with it, overwriting the old one"
        end
        system("cp '#{basedir}/#{file}' '#{output_helpfile}'")
        helphash[helpitem] = output_helpfile
      end
    end
  }
end

# process the parsed options

clean = false
confirmthings = true
linksDefaultHtml = $uselinks
opts = OptionParser.new do |opts|
  # Mandatory argument.
  opts.on("-s", "--source-dir directory",
    "A Source Directory for Help Files", "you can use this many times to give more than one source directory") do |dir|
    help_source_list.push(dir)
  end

  opts.on("-d", "--dest-dir directory",
    "The Destination Directory for the processed Help Files", "(defaults to #{help_dest})") do |dir|
    help_dest = dir
  end

  # Boolean switch.
  opts.on("-c", "--clean", "Clean the Destination Directory before building the helpfiles") do |c|
    clean = c
  end

  # Boolean switch.
  opts.on("-f", "--force", "Do not prompt to see if the user is sure about deleting files") do |y|
    confirmthings = !y
  end

  if linksDefaultHtml
    explain = "Use links to remove html formatting [default]"
  else
    explain = "Use links to remove html formatting"
  end
  # Boolean switch.
  opts.on("-l", "--links", explain) do |c|
    $uselinks = c
  end

  # Boolean switch.
  if not linksDefaultHtml
    explain = "Use unhtml to remove html formatting [default]"
  else
    explain = "Use unhtml to remove html formatting"
  end
  opts.on("-u", "--unhtml", explain) do |c|
    $uselinks = !c
  end

  # No argument, shows at tail.  This will print an options summary.
  opts.on_tail("-h", "--help", "Show this message") do
    puts opts
    exit
  end
end
opts.parse!(ARGV)

if help_source_list.length == 0
  puts "\n**You must provide at least one help Source Directory**\n\n"
  puts opts
  exit
end

#make sure the dest dir is valid
if !File.exists?(help_dest) 
  if(confirmthings)
    puts "Help Destination does not exist, should it be created? [yes/no]"
    responce = STDIN.readline
    responce.chomp!
    responce.gsub!(/\s*/,"")
  end
  if (!confirmthings) || responce == "y" || responce == "yes"
    newdirs = help_dest.split("/")
    if newdirs[0] = ""
      newdirs[1] = "/" + newdirs[1]
      newdirs = newdirs[1,newdirs.length]
    end
    basedir = ""
    newdirs.each{ |dir|
      basedir += "/#{dir}"
      if !File.exists?(basedir)
        Dir.mkdir(basedir)
      elsif  !File.stat(basedir).directory?
        puts "#{basedir} exists but is not a valid directory, please supply a valid Destination directory\n"
      end
    }
  else
    puts "Responce was not yes, aborting"
    exit
  end
elsif !File.stat(help_dest).directory?
  puts "Help Destination is not a directory, please supply a valid directory\n\n"
  puts opts
  exit
end

#make sure that the sources are valid
help_source_list.each { |help_source|
  if !File.exists?(help_source) || !File.stat(help_source).directory?
    puts "Help Source #{help_source} is not a directory, please supply a valid directory\n\n"
    puts opts
    exit
  end
}


#clean, removes everything from the help_destination, including the TAGS_HELP and sc_help_completion files
if clean
  if(confirmthings)
    puts "Clean option, will delete contents of #{help_dest}, are you sure you want to do this? [yes/no]"
    responce = STDIN.readline
    responce.chomp!
    responce.gsub!(/\s*/,"")
  end
  if (!confirmthings) || responce == "y" || responce == "yes"
    puts "Deleting contents of #{help_dest}"
    items_to_del = Dir.entries(help_dest)
    #remove . and .. and .svn
    items_to_del.delete('.'); items_to_del.delete('..') ; items_to_del.delete('.svn')
    items_to_del.delete('SCVim.scd')
    #add the path to the file names
    items_to_del.collect! {|x| "#{help_dest}/" + x }
    deleteRecursively(items_to_del)
  else
    puts "Responce was not yes, aborting"
    exit
  end
else
  #something intelligent should be said here about when new files will be created in the destdir
end

#read in the existing tags, if there are any, and add them to the help hash
if File.exists?(help_tags_file)
  File.open(help_tags_file, "r"){ |tags_file|
    tags_file.each {| line |
      line = line.split("\t")
        helphash[line[0].sub(/SC:/,"")] = line[1]
    }
  }
end

help_source_list.each { |help_source|
  puts "Grabbing Help from: #{help_source},\n\tprocessing it and putting it here: #{help_dest}"

  #get orig dir and chdir to the source
  orig_dir = Dir.pwd
  Dir.chdir(help_source)

  #find the files/directories in the source directory
  files = Dir.entries(help_source)
  files.delete('.') ; files.delete('..'); files.delete('.svn')
  #actually process the files [recursively]
  processFiles(files, ".", helphash, help_dest)
  #change directories back
  Dir.chdir(orig_dir)
}

#add SCVim.scd into the help output if it doesn't already exist there
if not helphash["SCVim"]
  helphash["SCVim"] = "SCVim.scd"
end

#write out the tags and completion files
completion = File.open(help_completion_file, "w")
File.open(help_tags_file, "w"){ |tags_file|
  helphash.sort.each {| key, val |
    val.sub!(/^#{help_dest}\//,"")
    tags_file.puts "SC:#{key}\t#{val}\t/^"
    #put the key into the completion file
    completion.puts key
  }
}

completion.close
#delete empty directories
items_to_check = Dir.entries(help_dest)
#remove . and ..
items_to_check.delete("."); items_to_check.delete(".."); items_to_check.delete(".svn")
items_to_check.collect! {|x| "#{help_dest}/" + x }
deleteEmptyDirs(items_to_check)

puts "Done"
