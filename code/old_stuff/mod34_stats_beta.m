function table=mod34_stats_beta(base_dir)
%	table=mod34_stats_beta_cb(33000000,500,10000,base_dir);
	table=mod34_stats_main(@(growthrate,meltrate,droprate)...
		mod34_stats_beta_cb(growthrate,meltrate,droprate,base_dir));
end

function table_row=mod34_stats_beta_cb(growthrate,meltrate,droprate,base_dir)
	dir=sprintf('%s/%d-%d-%d',base_dir,growthrate,meltrate,droprate);

	[tau,N,beta_cloud,alpha_cloud]=beta_comp(dir,'frame');

	i_find=find(beta_cloud);
	[beta_final_c,ratio_c]=limit_estimate(tau(i_find),beta_cloud(i_find));
	fprintf(2,'%.7g %.7g\n',beta_final_c,ratio_c);
	fflush(2);

	i_find=find(alpha_cloud);
	[alpha_final_c,alpha_ratio_c]=limit_estimate(tau(i_find),alpha_cloud(i_find));
	fprintf(2,'%.7g %.7g\n',alpha_final_c,alpha_ratio_c);
	fflush(2);

	[~,~,beta_dropped,alpha_dropped]=beta_comp(dir,'frame-dropped');

	i_find=find(beta_dropped);
	[beta_final_d,ratio_d]=limit_estimate(tau(i_find),beta_dropped(i_find));
	fprintf(2,'%.7g %.7g\n',beta_final_d,ratio_d);
	fflush(2);

	i_find=find(alpha_dropped);
	[alpha_final_d,alpha_ratio_d]=limit_estimate(tau(i_find),alpha_dropped(i_find));
	fprintf(2,'%.7g %.7g\n',alpha_final_d,alpha_ratio_d);
	fflush(2);


	table_row=[growthrate,meltrate,droprate...
		,beta_final_c,ratio_c...
		,beta_final_d,ratio_d,max(tau)...
		,alpha_final_c,alpha_ratio_c,alpha_final_d,alpha_ratio_d];
end
