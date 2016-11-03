function [stats]=aggregates_generate(paramstruct,exepath)
	[~,nowstring]=system('date --rfc-3339=ns --utc');
	nowstring=strtrim(nowstring);
	nowstring=strsplit(nowstring,'+');
	nowstring=nowstring{1};
	nowstring=regexprep(nowstring,'\s','_');
	nowstring=regexprep(nowstring,'[:+.]','-');

	statefile=ternary(@()isfield(paramstruct,'statefile')...
		,@()['--statefile=',paramstruct.statefile]...
		,@()'');

	stop_cond=ternary(@()isfield(paramstruct,'stop_cond')...
		,@()['--stop-cond=',paramstruct.stop_cond]...
		,@()'');

	shape=ternary(@()isfield(paramstruct,'shape')...
		,@()['--shape=',paramstruct.shape]...
		,@()'');

	deformations={};
	if isfield(paramstruct,'deformations')
		for k=1:numel(paramstruct.deformations)
			def=paramstruct.deformations{k};
			str=['--deformation=',def{1},',',def{2}];
			for l=3:numel(def)
				str=[str,',',num2str(def{l})];
			end
			deformations{k}=str;
		end
	end

	prototype_choices='';
	if isfield(paramstruct,'prototype_choices')
		if ischar(paramstruct.prototype_choices)
			prototype_choices=['--prototype-choises=',paramstruct.prototype_choices];
		else
			choices_file=['choices_',nowstring,'.json'];
			prototypechoices_write(choices_file,paramstruct.prototype_choicse);
			prorotype_choices=['--prototype-choises=',choices_file];
		end
	end

	output_dir=ternary(@()isfield(paramstruct,'output_dir')...
		,@()['--output-dir=',paramstruct.output_dir]...
		,@()'');

	dump_stats=ternary(@()isfield(paramstruct,'dump_stats')...
		,@()['--dump-stats=',num2str(paramstruct.dump_stats)]...
		,@()'');

	dump_geometry=ternary(@()isfield(paramstruct,'dump_geometry')...
		,@()['--dump-geometry=',paramstruct.dump_geometry]...
		,@()'');

	dump_geometry_ice=ternary(@()isfield(paramstruct,'dump_geometry_ice')...
		,@()['--dump-geometry-ice=',paramstruct.dump_geometry_ice]...
		,@()'');

	sample_geometry=ternary(@()isfield(paramstruct,'sample_geometry')...
		,@()['--sample_gemetry=',num2str(paramstruct.sample_geometry(1)),','...
				,num2str(paramstruct.sample_geometry(2)),','...
				,num2str(paramstruct.sample_geometry(3))] ...
		,@()'');

	seed=ternary(@()isfield(paramstruct,'seed')...
		,@()['--seed=',int2str(paramstruct.seed)]...
		,@()'');

	N=ternary(@()isfield(paramstruct,'N')...
		,@()['--N=',num2str(paramstruct.N)]...
		,@()'');

	droprate=ternary(@()isfield(paramstruct,'droprate')...
		,@()['--droprate=',num2str(paramstruct.droprate)]...
		,@()'');

	meltrate=ternary(@()isfield(paramstruct,'meltrate')...
		,@()['--meltrate=',num2str(paramstruct.meltrate)]...
		,@()'');

	growthrate=ternary(@()isfield(paramstruct,'growthrate')...
		,@()['--growthrate=',num2str(paramstruct.growthrate)]...
		,@()'');

	merge_retries=ternary(@()isfield(paramstruct,'merge_retries')...
		,@()['--merge-retries=',num2str(paramstruct.merge_retries)]...
		,@()'');

	overlap_min=ternary(@()isfield(paramstruct,'overlap_min')...
		,@()['--overlap-min=',num2str(paramstruct.overlap_min)]...
		,@()'');

	overlap_max=ternary(@()isfield(paramstruct,'overlap_max')...
		,@()['--overlap-max=',int2str(paramstruct.overlap_max)]...
		,@()'');

	n=nargin();
	cmd=ternary(@()(n<2),@()'snowflake_simulate3'...
		,@()[exepath,'/snowflake_simulate3'])

	system_wrapper([cmd,statefile,stop_cond,shape,deformations...
		,prototype_choices,output_dir,dump_stats,dump_geometry...
		,dump_geometry_ice,sample_geometry,seed,N,growthrate...
		,droprate,meltrate,merge_retries,overlap_min,overlap_max]...
		,nargout()>0);
	if ~isempty(output_dir)
		stats=csvread2(output_dir,'/frame_data.txt','\t');
	end
end

